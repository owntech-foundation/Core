import os
Import("env")

# Retrieve project options
board = env.GetProjectOption("board")
version = env.GetProjectOption("board_version", "")
shield = env.GetProjectOption("board_shield", "")
shield_version = env.GetProjectOption("board_shield_version", "")

cmake_file_path = os.path.join('.', "zephyr", "CMakeLists.txt")

def modify_cmake_file(file_path, board_name, bversion, shield_name, sversion):
    # Construct the full names with versions if provided
    full_board_name = f"{board_name}@{bversion}" if bversion else board_name
    full_shield_name = f"{shield_name}_v{sversion}" if sversion else shield_name

    # Read the CMakeLists.txt file
    try:
        with open(file_path, 'r') as file:
            original_lines = file.readlines()
    except FileNotFoundError:
        print(f"Error: The file {file_path} was not found.")
        return
    except IOError:
        print(f"Error: An IOError occurred while trying to read the file {file_path}.")
        return

    # Flags to check if the current values match
    board_needs_update = True
    shield_needs_update = True

    # Modify the lines accordingly, placing BOARD and SHIELD before CMAKE_VERBOSE_MAKEFILE
    new_lines = []
    board_set = False
    cmake_verbose_index = None

    for i, line in enumerate(original_lines):
        # Locate CMAKE_VERBOSE_MAKEFILE position
        if line.strip().startswith('set(CMAKE_VERBOSE_MAKEFILE '):
            cmake_verbose_index = i  # Track where CMAKE_VERBOSE_MAKEFILE is set
            break

    # Insert BOARD and SHIELD before CMAKE_VERBOSE_MAKEFILE if it exists
    for line in original_lines:
        if line.strip().startswith('set(BOARD '):
            # Update the BOARD line if necessary
            if full_board_name not in line:
                new_lines.append(f'set(BOARD {full_board_name})\n')
            else:
                new_lines.append(line)  # Retain existing BOARD if it's already correct
            board_set = True
        elif line.strip().startswith('set(SHIELD '):
            # Skip the existing SHIELD line to replace it with the new one
            continue
        else:
            new_lines.append(line)

    # Add the BOARD directive if it was not found
    if not board_set:
        new_lines.append(f'set(BOARD {full_board_name})\n')

    # Add the SHIELD directive (only once, and right after BOARD if present)
    if shield_name:
        # Insert the SHIELD right after BOARD if possible
        for idx, line in enumerate(new_lines):
            if 'set(BOARD ' in line:
                new_lines.insert(idx + 1, f'set(SHIELD {full_shield_name})\n')
                break
        else:
            # If no BOARD is present, just append SHIELD at the end
            new_lines.append(f'set(SHIELD {full_shield_name})\n')

    # Write the modified lines back to the file only if changes were made
    if original_lines != new_lines:
        try:
            with open(file_path, 'w') as file:
                file.writelines(new_lines)
            print(f"Updated {file_path} with new board and shield values.")
        except IOError:
            print(f"Error: An IOError occurred while trying to write to the file {file_path}.")
    else:
        print("No changes made to CMakeLists.txt, skipping update.")

# Run the modify_cmake_file function with the board and shield info
modify_cmake_file(cmake_file_path, board, version, shield, shield_version)
