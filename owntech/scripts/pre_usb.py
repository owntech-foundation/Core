import os

def stlink_to_usb():
    # Specify the dts path
    dts_path = os.path.join(".", "zephyr", "boards", "owntech", "spin", "spin.dts")

    # Define the search and replace strings
    search_str = "zephyr,console = &lpuart1;"
    replace_str = "zephyr,console = &cdc_acm_uart0;"

    # Read the contents of the file
    with open(dts_path, 'r') as file:
        file_contents = file.read()

    # Check if the search string is present in the file
    if search_str in file_contents:
        # Replace the search string with the replace string
        new_contents = file_contents.replace(search_str, replace_str)

        # Write the updated contents back to the file
        with open(dts_path, 'w') as file:
            file.write(new_contents)

        print(f"Replacement of '{search_str}' with '{replace_str}' in '{dts_path}' completed.")
    else:
        print(f"'{search_str}' not found in '{dts_path}'. No replacement performed.")

stlink_to_usb()