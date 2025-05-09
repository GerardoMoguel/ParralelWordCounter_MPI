import tkinter as tk
from tkinter import filedialog, simpledialog, messagebox

def main():
    # Hide the main Tk window
    root = tk.Tk()
    root.withdraw()

    try:
        # Ask how many files the user wants to select
        count = simpledialog.askinteger("Input", "How many TXT files do you want to select?", minvalue=1)
        if count is None:
            messagebox.showinfo("Cancelled", "You cancelled the selection.")
            return

        file_paths = []

        # Open file dialog `count` times
        for i in range(count):
            path = filedialog.askopenfilename(
                title=f"Select TXT file {i + 1}",
                filetypes=[("Text files", "*.txt")],
            )
            if path:
                file_paths.append(path)
            else:
                messagebox.showwarning("Cancelled", f"You cancelled at file {i + 1}. Stopping.")
                return

        # Show all selected paths
        print("Selected file paths:")
        for p in file_paths:
            print(p)

    except Exception as e:
        messagebox.showerror("Error", str(e))

if __name__ == "__main__":
    main()