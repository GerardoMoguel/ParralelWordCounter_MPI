""""
Gerardo Moguel


============================================================================================
This class is the controller of the whole project, it does the next services:
--Mantains the whole GUI
--Reads the path of the .txt files
--Orders the c++ script to execute
--Interpret the results from the c++ script
--Plots the histogram of words, and the speedup graphs
============================================================================================
TL;DR = This is the controller that communicates between the GUI and .cpp scripts


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
from matplotlib.figure import Figure
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg



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
                              MAIN TAB METHODS (n methods)
    ================================================================================
    
    1.
    ================================================================================
                                    <Analize Function>
      It initializes the BagOfWords.cpp script, giving the selected_paths.txt file
                                       as parameter.
    ================================================================================
    """
    def analizeBtn(button):
        # Path to the compiled C++ executable
        exe_path = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Executables\BagOfWords.exe" 
        exe_path2 = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Executables\WordCounters.exe" 
        # Path to the .txt file you want to pass as argument
        paths_file = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\selected_paths.txt" 
        map_file = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\file_maps.txt"
        button.pack(padx=20,pady=10)
        # Run the .exe with the file as argument
        subprocess.run([exe_path, paths_file])
        subprocess.run(["mpiexec", "-n", "6", exe_path2, map_file, paths_file])
        # Paths to your data files
        file_map_path = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\file_map_serial.txt"
        times_path = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\execution_times.txt"

        # ─── 1) Load & process word-frequency data ────────────────────────────────────
        df = pd.read_csv(file_map_path, delim_whitespace=True)
        book_cols = [c for c in df.columns if c != "word"]
        df["total"] = df[book_cols].sum(axis=1)
        threshold = 100  # only show words with total > threshold
        mask_num = ~df["word"].str.match(r"^\d+$", na=False)
        mask_freq = df["total"] > threshold
        df_filtered = df[mask_num & mask_freq]

        # compute top 3 words
        top3 = df_filtered.nlargest(3, "total")[["word", "total"]]

        # ─── 2) Load execution times ───────────────────────────────────────────
        times_df = pd.read_csv(times_path, delim_whitespace=True)
        times_long = times_df.melt(var_name="Mode", value_name="Time (s)")

        # ─── Words Histogram Tab ───────────────────────────────────────────────
        fig1 = Figure(figsize=(6, 4), dpi=100)
        ax1 = fig1.add_subplot(111)
        # horizontal histogram: freq on Y, count of words on X
        ax1.hist(df_filtered["total"], bins=50, orientation="horizontal")
        ax1.set_title(f"Word Frequencies (total > {threshold})")
        ax1.set_xlabel("Number of Words")
        ax1.set_ylabel("Total Frequency")

        canvas1 = FigureCanvasTkAgg(fig1, master=tabview.tab("Words Histogram"))
        canvas1.draw()
        canvas1.get_tk_widget().pack(expand=True, fill="both")

        # — Display top 3 words below the plot —
        top_text = "\n".join(
            f"{i+1}. {row.word} ({row.total})"
            for i, row in top3.iterrows()
        )
        ctk.CTkLabel(
            master=tabview.tab("Words Histogram"),
            text=f"Top 3 Words:\n{top_text}",
            justify="left"
        ).pack(padx=10, pady=10, anchor="w")

        # ─── Speedup Graph Tab ─────────────────────────────────────────────────
        fig2 = Figure(figsize=(6, 4), dpi=100)
        ax2 = fig2.add_subplot(111)
        ax2.bar(times_long["Mode"], times_long["Time (s)"])
        ax2.set_title("Execution Times: Serial vs Parallel")
        ax2.set_ylabel("Time (s)")

        canvas2 = FigureCanvasTkAgg(fig2, master=tabview.tab("Speedup Graph"))
        canvas2.draw()
        canvas2.get_tk_widget().pack(expand=True, fill="both")
        
    """"
    2.
    ================================================================================
                                    <Click Function>
      At some part, this method will "forget" a button that isn't instantiated yet, 
       because that button uses this method to work, its a mutual dependency.
                Thats the reason why it has a button as the parameter
    ================================================================================
    TL;DR = It has necessary button as parameter
    """
    file_paths = []
    def click(button,button2):
        try:
            try:
                count = int(txtFld1.get("1.0", "end").strip()) #Receive the number of books from the user
            except ValueError:
                lbl3.configure(text="Please enter a valid integer.") 
                return

            if count <= 0:
                lbl3.configure(text="Count must be positive.")
                return

            #file_paths = []

            for i in range(count): #receive the path from each book.txt
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
            output_text = "⤷Files selected⤶\n\n" + "\n\n".join(file_paths)
            lbl3.configure(text=output_text)

            # Save to file
            output_path = r"D:\Documentos\GitHub\ParralelWordCounter_MPI\Outputs\selected_paths.txt" #put the path to your outputs folder
            with open(output_path, "w", encoding="utf-8") as f:
                f.write(str(count) + "\n")
                for path in file_paths:
                    f.write(path + "\n")

            button.pack_forget() #no more counting books
            txtFld1.pack_forget() #no more console writing >:o
            lbl2.pack_forget() #no warm text asking you to write the input
            lbl1.configure(text="Great!, now click on 'Analize' to begin the process of analisis.\n ")
            button2.pack(padx=20, pady=10)

        except Exception as e:
            messagebox.showerror("Error", str(e))





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

    # Textbox
    txtFld1 = ctk.CTkTextbox(master=main_frame, corner_radius=16, border_width=2, width=300, height=50) #make the textbox roundy
    txtFld1.pack(padx=20, pady=10)

    # Analize button
    btn2 = ctk.CTkButton(master=main_tab, text="Analize",command=lambda: analizeBtn(btn2))
    btn2.pack(padx=20, pady=10)
    btn2.pack_forget()

    # Select files button
    btn1 = ctk.CTkButton(master=main_tab, text="Select Files", command=lambda: click(btn1,btn2))
    btn1.pack(padx=20, pady=10)


    """
    ================================================================================
                                WORDS HISTOGRAM TAB
    ================================================================================
    """
    histo_tab = tabview.tab("Words Histogram")
    histo_label = ctk.CTkLabel(master=histo_tab, text="Descripción para seleccionar gráficas.")
    histo_label.pack(padx=20, pady=20)
    #this has to be built, until we finish executing the .cpp scripts.

    """
    ================================================================================
                                SPEEDUP GRAPH TAB
    ================================================================================
    """
    speedup_tab = tabview.tab("Speedup Graph")
    speedup_label = ctk.CTkLabel(master=speedup_tab, text="Descripción de la gráfica de speedup.")
    speedup_label.pack(padx=20, pady=20)

    #keeps the app running
    app.mainloop()


if __name__ == "__main__":
    main()