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
 * File: slide_window.h
 * Author: Civic_Crab
 * Version: 0.0.1
 * Created on: 2025-5-11
 *
 * Description:
 * This file contains the slide window data structure and its related functions.
 */

 #ifndef SLIDE_WINDOW_H
 #define SLIDE_WINDOW_H

 //Includes
#include <stdint.h>
 //End Includes

 //Defines
 #define SLIDE_WINDOW_MAX_SIZE 500
 //End Defines

 //Typedefs
typedef int32_t Slide_Window_Data_Type;
typedef uint16_t Slide_Window_Size_Type;
 //End Typedefs

 //Structs
typedef struct Slide_Window
 {
    Slide_Window_Data_Type data[SLIDE_WINDOW_MAX_SIZE];
    Slide_Window_Size_Type size;
    Slide_Window_Size_Type front;
    Slide_Window_Size_Type rear;
 }Slide_Window;
 //End Structs

 //Public Functions
void slide_window_init(Slide_Window *window);
void slide_window_add(Slide_Window *window, Slide_Window_Data_Type data);
void slide_window_retrieve(Slide_Window *window, Slide_Window_Data_Type *data,Slide_Window_Size_Type index);
 //End Public Functions

 #endif //SLIDE_WINDOW_H