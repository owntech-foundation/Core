import os
Import("env")

# Retrieve project options
# version default is empty
# shield default is empty
board = env.GetProjectOption("board")
version = env.GetProjectOption("board_version", "")
shield = env.GetProjectOption("board_shield", "")
shield_version = env.GetProjectOption("board_shield_version", "")

cmake_file_path = os.path.join('.', "zephyr", "CMakeLists.txt")

def modify_cmake_file(file_path, board_name, bversion, shield_name, sversion):
    # Construct the full names with versions if provided
    full_board_name = f"{board_name}@{bversion}" if bversion else board_name
    full_shield_name = f"{shield_name}_v{sversion}" if sversion else shield_name

    try:
        # Read the CMakeLists.txt file
        with open(file_path, 'r') as file:
            lines = file.readlines()
    except FileNotFoundError:
        print(f"Error: The file {file_path} was not found.")
        return
    except IOError:
        print(f"Error: An IOError occurred while trying to read the file {file_path}.")
        return

    # Flags to check if the directives were found
    board_set = False
    shield_set = False
    board_index = -1

    # Modify the lines accordingly
    new_lines = []
    for i, line in enumerate(lines):
        if line.strip().startswith('set(BOARD '):
            new_lines.append(f'set(BOARD {full_board_name})\n')
            board_set = True
            board_index = i
        elif line.strip().startswith('set(SHIELD '):
            if shield_name:  # Only add the shield line if shield_name is not empty
                new_lines.append(f'set(SHIELD {full_shield_name})\n')
                shield_set = True
            # Do not add the existing shield line if shield_name is empty
        else:
            new_lines.append(line)

    # Add the directives if they were not found
    if not board_set:
        new_lines.append(f'set(BOARD {full_board_name})\n')
        print(f"Added 'set(BOARD {full_board_name})' to {file_path}.")
        board_index = len(new_lines) - 1

    if shield_name and not shield_set:
        # Insert shield line right after the board line
        if board_index != -1:
            new_lines.insert(board_index + 1, f'set(SHIELD {full_shield_name})\n')
        else:
            new_lines.append(f'set(SHIELD {full_shield_name})\n')
        print(f"Added 'set(SHIELD {full_shield_name})' to {file_path}.")

    try:
        # Write the modified lines back to the file
        with open(file_path, 'w') as file:
            file.writelines(new_lines)
    except IOError:
        print(f"Error: An IOError occurred while trying to write to the file {file_path}.")

modify_cmake_file(cmake_file_path, board, version, shield, shield_version)
