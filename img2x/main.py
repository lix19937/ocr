
# -*- coding: UTF-8 -*-
import os
import cv2
 
def picvideo(img_path, size):
    filelist = os.listdir(img_path)  
 
    fps = 24//24
    video_path = "t.avi"  
    fourcc = cv2.VideoWriter_fourcc('I','4','2','0')  # 'I','4','2','0' map to avi 
    video = cv2.VideoWriter(video_path, fourcc, fps, size, True)
 
    for i in range(len(filelist)):
        item = os.path.join(img_path, str(i)+'.png')
        img = cv2.imread(item)  # img type is numpy.ndarray obj, BGR order 
        video.write(img)  
 
    video.release() 
    print('video_path:', video_path)
    return video_path


def main2video(img_path): 
    img_list = os.listdir(img_path)
    print("img_list len:", len(img_list))

    img = cv2.imread(os.path.join(img_path, img_list[0]))
    w, h, _ = img.shape
    return picvideo(img_path, (h, w))  


import imageio
def create_gif(img_path, gif_name, duration = 1.0):
    frames = []
    for image_name in os.listdir(img_path):
        temp = os.path.join(img_path, image_name)
        print(temp)
        frames.append(imageio.imread(temp))

    imageio.mimsave(gif_name, frames, 'GIF', duration=duration)
    return

def main2gif(image_path):
    gif_name = 't.gif'
    duration = 0.08  # interval time
    create_gif(image_path, gif_name, duration)

if __name__ == '__main__':
  main2video('imgs')
  main2gif('imgs')
  
