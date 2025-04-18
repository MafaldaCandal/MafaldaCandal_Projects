#!/usr/bin/env python3

"""
Assembler for the Hack Assembly Language

This module translates Hack assembly language into 16-bit binary machine code.
It implements a two-pass assembly process using the following classes:

- Parser:
  Processes the input assembly code by stripping whitespace and comments,
  and parses commands into their individual parts (A, C, and L commands).

- BinaryCode:
  Converts Hack assembly language mnemonics (dest, comp, jump) into their
  corresponding binary representations using static mapping dictionaries.

- SymbolTable:
  Maintains a mapping between symbols (including predefined symbols, labels, and variables)
  and their memory addresses, allowing for symbol resolution during translation.

- Assembler:
  Implements the two-pass process: the first pass records label declarations (L_COMMANDs)
  into the symbol table, and the second pass translates A_COMMANDs and C_COMMANDs into binary code.
"""

import sys

class Parser:
    """
    The Parser class processes the input assembly code.
    It removes whitespace/comments, and provides methods
    to parse each command into its constituent parts.
    """
    
    def __init__(self, input_data):
        """
        Initialize the Parser with non-empty, comment-stripped lines from input_data.
        Sets the current_line counter to 0.
        """
        unprocessed_lines = input_data.splitlines()
        self.lines = []
        for line in unprocessed_lines:
            line = line.split('//')[0].strip()
            if line:
                self.lines.append(line)
        self.current_line = 0
        
    def hasMoreCommands(self):
        """
        Returns True if current_line is smaller to the number of lines,
        meaning there are more lines of assembly code to analyse.
        """
        return self.current_line < len(self.lines)
    
    def advance(self):
        """Updates the current command, and increases current_line by one."""
        self.current_command = self.lines[self.current_line]
        self.current_line += 1
        
    def reset_count(self):
        """
        Define current_line as 0, setting up a new iteration
        through the lines of assembly code.
        """
        self.current_line = 0
        
    def commandType(self):
        """
        Returns the type of the current command:
        - 'A_COMMAND' for @Xxx (where Xxx is either a symbol or a decimal number)
        - 'C_COMMAND' for dest=comp;jump
        - 'L_COMMAND' for (Xxx) (pseudo-command for label declarations)
        """
        cmd = self.current_command
        if cmd.startswith('@'):
            return 'A_COMMAND'
        elif cmd.startswith('(') and cmd.endswith(')'):
            return 'L_COMMAND'
        else:
            return "C_COMMAND"
        
    def symbol(self):
        """
        Returns the symbol or decimal Xxx of the current command-removes the "@" or parenthesis.
        Should be called only when commandType() is A_COMMAND or L_COMMAND.
        """
        cmd_type = self.commandType()
        cmd = self.current_command
        match cmd_type:
            case 'A_COMMAND':
                return cmd[1:]
            case 'L_COMMAND':
                return cmd[1:-1]
            case _:
                raise ValueError(f"Invalid command type for symbol extraction: {self.commandType()}")
  
    def dest(self):
        """
        Returns the destination mnemonic in the current C-command.
        Should be called only when commandType() is C_COMMAND.
        """
        cmd_type = self.commandType()
        cmd = self.current_command
        match (cmd_type, cmd):
            case ('C_COMMAND', command) if '=' in command:
                return cmd.split('=')[0]
            case ('C_COMMAND', _):
                return ''
            case _:
                raise ValueError(f"Invalid command type for dest extraction: {cmd_type}")

    def comp(self):
        """
        Returns the comp mnemonic in the current C-command.
        Should be called only when commandType() is C_COMMAND.
        """
        cmd_type = self.commandType()
        match cmd_type:
            case 'C_COMMAND':
                comp_field = self.current_command
                if '=' in comp_field:
                    comp_field = comp_field.split('=')[1]
                if ';' in comp_field:
                    comp_field = comp_field.split(';')[0]
                return comp_field
            case _:
                raise ValueError(f"Invalid command type for comp extraction: {self.commandType()}")
    
    def jump(self):
        """
        Returns the jump mnemonic in the current C-command.
        Should be called only when commandType() is C_COMMAND.
        """
        cmd_type = self.commandType()
        cmd = self.current_command
        match (cmd_type, cmd):
            case ('C_COMMAND', command) if ';' in command:
                return command.split(';', 1)[1]
            case ('C_COMMAND', _):
                return ''
            case _:
                raise ValueError(f"Invalid command type for jump extraction: {cmd_type}")
    
    
class BinaryCode:
    """
    The BinaryCode class translates Hack assembly mnemonics into their corresponding
    binary codes.
    """
    
    # Mapping for dest mnemonics
    dest_dict = {
        '':    '000',
        'M':   '001',
        'D':   '010',
        'MD':  '011',
        'A':   '100',
        'AM':  '101',
        'AD':  '110',
        'AMD': '111'
    }

    # Mapping for comp mnemonics (both for A and M, see spec for a-bit handling)
    comp_dict = {
        # when a=0
        '0':   '0101010',
        '1':   '0111111',
        '-1':  '0111010',
        'D':   '0001100',
        'A':   '0110000',
        '!D':  '0001101',
        '!A':  '0110001',
        '-D':  '0001111',
        '-A':  '0110011',
        'D+1': '0011111',
        'A+1': '0110111',
        'D-1': '0001110',
        'A-1': '0110010',
        'D+A': '0000010',
        'D-A': '0010011',
        'A-D': '0000111',
        'D&A': '0000000',
        'D|A': '0010101',
        # when a=1
        'M':   '1110000',
        '!M':  '1110001',
        '-M':  '1110011',
        'M+1': '1110111',
        'M-1': '1110010',
        'D+M': '1000010',
        'D-M': '1010011',
        'M-D': '1000111',
        'D&M': '1000000',
        'D|M': '1010101'
    }

    # Mapping for jump mnemonics
    jump_dict = {
        '':    '000',
        'JGT': '001',
        'JEQ': '010',
        'JGE': '011',
        'JLT': '100',
        'JNE': '101',
        'JLE': '110',
        'JMP': '111'
    }
    
    @staticmethod
    def dest(mnemonic):
        """Returns the 3-bit binary code for the dest mnemonic."""
        if mnemonic not in BinaryCode.dest_dict:
            raise KeyError(f"Invalid mnemonic: {mnemonic}")
        return BinaryCode.dest_dict[mnemonic]

    @staticmethod
    def comp(mnemonic):
        """Returns the 7-bit binary code for the comp mnemonic."""
        if mnemonic not in BinaryCode.comp_dict:
            raise KeyError(f"Invalid mnemonic: {mnemonic}")
        return BinaryCode.comp_dict[mnemonic]

    @staticmethod
    def jump(mnemonic):
        """Returns the 3-bit binary code for the jump mnemonic."""
        if mnemonic not in BinaryCode.jump_dict:
            raise KeyError(f"Invalid mnemonic: {mnemonic}")
        return BinaryCode.jump_dict[mnemonic]
    
    
class SymbolTable:
    """
    The SymbolTable class manages the symbols found in Hack assembly code,
    mapping each symbol to its corresponding numeric memory address.
    """
    
    def __init__(self):
        """Creates a new symbol table and initializes it with predefined symbols."""
        self.symboltable = {
            "SP": 0,
            "LCL": 1,
            "ARG": 2,
            "THIS": 3,
            "THAT": 4,
            "R0": 0,
            "R1": 1,
            "R2": 2,
            "R3": 3,
            "R4": 4,
            "R5": 5,
            "R6": 6,
            "R7": 7,
            "R8": 8,
            "R9": 9,
            "R10": 10,
            "R11": 11,
            "R12": 12,
            "R13": 13,
            "R14": 14,
            "R15": 15,
            "SCREEN": 16384,
            "KBD": 24576
        }
        
    def addEntry(self, symbol, address):
        """Adds the pair (symbol, address) to the symbol table."""
        self.symboltable[symbol] = address
        
    def contains(self, symbol):
        """Returns True if the symbol table contains the given symbol."""
        return symbol in self.symboltable
    
    def getAddress(self, symbol):
        """Returns the address associated with the given symbol."""
        return self.symboltable[symbol]
        
        
class Assembler:
    """
    The Assembler class translates Hack assembly language code into binary machine code.
    It uses a two-pass process:
      - The first pass scans the input and records label declarations (L_COMMANDs) with their ROM addresses.
      - The second pass translates A_COMMANDs and C_COMMANDs into their 16-bit binary representations,
        allocating new memory addresses for variables as needed.
    """

    def __init__(self, input_data):
        """
        Initializes a new Assembler instance with the provided assembly code.

        Reads the input data and creates the necessary parser and symbol table.
        """
        self.parser = Parser(input_data)
        self.symboltable = SymbolTable()

    def __first_pass(self):
        """
        For each command:
          - If the command is a label declaration (L_COMMAND), its label is added to the symbol table 
            with the current ROM address.
          - Otherwise, the ROM address counter is incremented.
        """
        rom_address = 0
        while self.parser.hasMoreCommands():
            self.parser.advance()
            match self.parser.commandType():
                case 'L_COMMAND':
                    label = self.parser.symbol()
                    if not self.symboltable.contains(label):
                        self.symboltable.addEntry(label, rom_address)
                case _:
                    rom_address += 1

    def __second_pass(self):
        """
        The parser is reset to the beginning and each command is processed:
          - A_COMMANDs are translated to a 16-bit binary value. If the symbol is not numeric,
            a new memory address is allocated if needed.
          - C_COMMANDs are broken into destination, computation, and jump parts and then
            converted into their binary representation using the BinaryCode module.
          - L_COMMANDs are ignored.
          
        Returns a list of 16-bit binary instruction strings.
        """
        self.parser.reset_count()
        variable_address = 16
        machine_instructions = []
        
        self.parser.reset_count()
        variable_address = 16
        machine_instructions = []
        while self.parser.hasMoreCommands():
            self.parser.advance()
            match self.parser.commandType():
                case 'A_COMMAND':
                    symbol = self.parser.symbol()
                    if symbol.isdigit():
                        address = int(symbol)
                    else:
                        if not self.symboltable.contains(symbol):
                            self.symboltable.addEntry(symbol, variable_address)
                            variable_address += 1
                        address = self.symboltable.getAddress(symbol)
                    machine_instructions.append(format(address, '016b'))
                case 'C_COMMAND':
                    dest_mnemonic = self.parser.dest()
                    comp_mnemonic = self.parser.comp()
                    jump_mnemonic = self.parser.jump()
                    binary_instruction = (
                        "111"
                        + BinaryCode.comp(comp_mnemonic)
                        + BinaryCode.dest(dest_mnemonic)
                        + BinaryCode.jump(jump_mnemonic)
                    )
                    machine_instructions.append(binary_instruction)
                case _:
                    # L_COMMANDs are ignored
                    pass
        return machine_instructions

    def translation(self):
        """Executes the full two-pass assembly process."""
        self.__first_pass()
        return self.__second_pass()
       
           
def main():
    input_data = sys.stdin.read()
    assembler = Assembler(input_data)
    machine_instructions = assembler.translation()

    # Output each 16-bit machine instruction on its own line.
    for instruction in machine_instructions:
        print(instruction)


if __name__ == "__main__":
    """
    This guard ensures that the code within it only runs
    when the script is executed directly, and not when it's imported as a module.
    """
    main()
