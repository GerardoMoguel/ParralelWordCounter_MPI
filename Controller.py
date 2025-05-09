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
        # Show paths in popup
        messagebox.showinfo("Files selected", "\n\n".join(file_paths))

        # Also save to a file in a specific folder
        output_path = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\selected_paths.txt"

        with open(output_path, "w", encoding="utf-8") as f:
            f.write(str(count) + "\n") #we put the counter first, that way the c++ file can use that value.
            for path in file_paths:
                f.write(path + "\n")

    except Exception as e:
        messagebox.showerror("Error", str(e))

if __name__ == "__main__":
    main()