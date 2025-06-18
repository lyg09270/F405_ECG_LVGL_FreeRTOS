/*
 * Copyright 2024 Civic_Crab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
 * File: slide_window.c
 * Author: Civic_Crab
 * Version: 0.0.1
 * Created on: 2025-5-11
 *
 * Description:
 * This file contains the implementation of the slide window data structure.
 */

//Includes
#include "slide_window.h"
#include <string.h>
//End Includes

//Private Functions

//End Private Functions

//Public Functions

//void slide_window_init(Slide_Window *window)
//Description:
//        This function Initializes all value in the slide window to 0.
//Args:
//        Slide_Window *window: Pointer to the slide window structure.
//Returns:
//        None
void slide_window_init(Slide_Window *window) 
{
    memset(window->data, 0, sizeof(window->data));
    window->size = 0;
    window->front = 0;
    window->rear = 0;
    return;
}

//void slide_window_add(Slide_Window *window, Slide_Window_Data_Type data)
//Description:
//        This function adds a new data to the slide window.
//Args:
//        Slide_Window *window: Pointer to the slide window structure.
//        Slide_Window_Data_Type data: The data to be added to the slide window.
//Returns:
//        None
void slide_window_add(Slide_Window *window, Slide_Window_Data_Type data)
{
    if(window->size < SLIDE_WINDOW_MAX_SIZE)
    {
        window->data[window->rear] = data;
        window->rear = (window->rear + 1) % SLIDE_WINDOW_MAX_SIZE;
        window->size++;
    }
    else if(window->size == SLIDE_WINDOW_MAX_SIZE)
    {
        window->data[window->rear] = data;
        window->rear = (window->rear + 1) % SLIDE_WINDOW_MAX_SIZE;
        window->front = (window->front + 1) % SLIDE_WINDOW_MAX_SIZE;
    }
}

//void slide_window_retrieve(Slide_Window *window, Slide_Window_Data_Type *data,Slide_Window_Size_Type index)
//Description:
//      Retrieve data from the slide window.
//Args:
//      window: the slide window.
//      data: the data to be retrieved.
//      index: the index of the data to be retrieved.
//return:
//      None
void slide_window_retrieve(Slide_Window *window, Slide_Window_Data_Type *data,Slide_Window_Size_Type index)
{ 
    if(window->size >= 0)
    {
        data[index] = window->data[(window->front + index) % SLIDE_WINDOW_MAX_SIZE];
    }
}

//End Public Functions

