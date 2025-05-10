""""
Gerardo Moguel


============================================================================================
This class is the controller of the whole project, it does the next services:
--Mantains the whole GUI
--Reads the directory of the .txt files
--Orders the c++ script to execute
--Interpret the results from the c++ script
--Plots the histogram of words, and the speedup graphs
============================================================================================



============================================================================================
                                        IMPORTS
============================================================================================
"""
import os
import time
import subprocess

import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns

import customtkinter as ctk
from customtkinter import *
from tkinter.filedialog import askopenfilenames
from tkinter import Tk, simpledialog, messagebox, filedialog

def main():
    """
    ================================================================================
                                CREATE MAIN WINDOW & TABS
    ================================================================================
    """
    # Create the main window
    app = ctk.CTk()
    app.geometry("1000x600")
    ctk.set_appearance_mode("dark")
    ctk.set_default_color_theme("dark-blue")

    # Create tab view
    tabview = ctk.CTkTabview(master=app)
    tabview.pack(padx=20, pady=20)
    tabview.add("Main tab")
    tabview.add("Words Histogram")
    tabview.add("Speedup Graph")

    """
    ================================================================================
                                        MAIN TAB
    ================================================================================
    """
    main_tab = tabview.tab("Main tab")

    # Wrapper frame for layout consistency
    main_frame = ctk.CTkFrame(master=main_tab)
    main_frame.pack(padx=20, pady=20)

    # Description label
    lbl1 = ctk.CTkLabel(master=main_frame, text="Esta es la\ndescripción al comienzo del app")
    lbl1.pack(padx=20, pady=5)

    # Prompt label
    lbl2 = ctk.CTkLabel(master=main_frame, text="How many books should I analyze?")
    lbl2.pack(padx=20, pady=5)

    # Info label
    lbl3 = ctk.CTkLabel(master=main_frame, text="Waiting for input...")
    lbl3.pack(padx=20, pady=5)

    # Entry
    txtFld1 = ctk.CTkTextbox(master=main_frame, corner_radius=16, border_width=2, width=300, height=50)
    txtFld1.pack(padx=20, pady=10)

    # Analize button
    btn2 = ctk.CTkButton(master=main_tab, text="Analize")
    btn2.pack(padx=20, pady=10)
    btn2.pack_forget()

    # Button click function
    def click(button):
        try:
            try:
                count = int(txtFld1.get("1.0", "end").strip())
            except ValueError:
                lbl3.configure(text="❌ Please enter a valid integer.")
                return

            if count <= 0:
                lbl3.configure(text="❌ Count must be positive.")
                return

            file_paths = []

            for i in range(count):
                path = filedialog.askopenfilename(
                    title=f"Select .txt file {i + 1}",
                    filetypes=[("Text files", "*.txt")],
                )
                if path:
                    file_paths.append(path)
                else:
                    messagebox.showwarning("Cancelled", f"You cancelled at file {i + 1}. Stopping.")
                    return

            # Show paths in label
            output_text = "✅ Files selected:\n\n" + "\n\n".join(file_paths)
            lbl3.configure(text=output_text)

            # Save to file
            output_path = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\selected_paths.txt"
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(str(count) + "\n")
                for path in file_paths:
                    f.write(path + "\n")

            button.pack_forget() 
            txtFld1.pack_forget()
            lbl2.pack_forget()
            lbl1.configure(text="Great!, now click on 'Analize' to begin the process of analisis.\n ")
            btn2.pack(padx=20, pady=10)
        except Exception as e:
            messagebox.showerror("Error", str(e))

    # Select files button
    comnd = click
    btn1 = ctk.CTkButton(master=main_tab, text="Select Files", command=lambda: click(btn1))
    btn1.pack(padx=20, pady=10)



    """
    ================================================================================
                                WORDS HISTOGRAM TAB
    ================================================================================
    """
    histo_tab = tabview.tab("Words Histogram")
    histo_label = ctk.CTkLabel(master=histo_tab, text="Descripción para seleccionar gráficas.")
    histo_label.pack(padx=20, pady=20)

    """
    ================================================================================
                                SPEEDUP GRAPH TAB
    ================================================================================
    """
    speedup_tab = tabview.tab("Speedup Graph")
    speedup_label = ctk.CTkLabel(master=speedup_tab, text="Descripción de la gráfica de speedup.")
    speedup_label.pack(padx=20, pady=20)

    app.mainloop()


if __name__ == "__main__":
    main()