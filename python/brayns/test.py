import tkinter
import threading
import pathlib
import time

import PIL.Image
from PIL import Image, ImageTk


class Viewer(threading.Thread):
    def __init__(self):
        super().__init__()
        self.window = None
        self.canvas = None
        self.curr_img = None
        self.thread_running_event = threading.Event()

    def close_window(self):
        self.thread_running_event.set()
        if self.window is not None:
            self.window.quit()
            self.curr_img = None
            self.canvas = None
            self.window = None

    def run(self):
        self.window = tkinter.Tk()
        self.window.title("Brayns python viewer")
        self.window.minsize(800, 600)
        self.window.geometry("800x600")
        self.canvas: tkinter.Canvas = tkinter.Canvas(master=self.window,
                                                     width=800,
                                                     height=600)
        self.canvas.configure(bg='black')
        self.canvas.pack()
        self.thread_running_event.set()
        self.window.mainloop()
        self.close_window()

    def set_image(self, img_path):
        self.thread_running_event.wait()
        if self.window is not None:
            img: PIL.Image.Image = Image.open(img_path)

            img_width = img.width
            img_height = img.height

            win_width = self.window.winfo_width()
            win_height = self.window.winfo_height()

            if img_width != win_width or img_height != win_height:
                current_aspect_ratio = img_width / img_height
                if win_width > win_height:
                    img_width = win_width
                    img_height = int(img_width / current_aspect_ratio)
                else:
                    img_height = win_height
                    img_width = int(img_height * current_aspect_ratio)

            img_pos_x = abs(win_width - img_width) * 0.5
            img_pos_y = abs(win_height - img_height) * 0.5
            img = img.resize(size=(img_width, img_height))
            self.curr_img = ImageTk.PhotoImage(img)
            self.canvas.create_image(img_pos_x, img_pos_y, anchor=tkinter.NW, image=self.curr_img)
            self.window.update()


app_thread = Viewer()
app_thread.start()
wallpapers_dir = pathlib.Path("/home/nadir/Pictures/Wallpapers")

for p in wallpapers_dir.iterdir():
    print("Setting {}".format(p))
    app_thread.set_image(str(p))

app_thread.join()