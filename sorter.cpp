#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <thread>
#include <cmath>
#include <chrono>

#define TICKS_PER_FRAME (1)

//global variables
const int WIDTH=1080, HEIGHT=1080, FRAME_DELAY = 1, RUN = 32, LIST_SIZE = 1080;
const char *windowTitle="Sorter";

//multithread time checking variable
int lastTime=0;

//important variables for multithread communication
bool isSorting = false;
bool proceed = false;
char choice;
bool run = true;

//window and renderer
SDL_Window *window=nullptr;
SDL_Renderer *renderer = nullptr;
SDL_Texture *instructions = nullptr;


//delay function
void Delay(int d){
    while(SDL_GetTicks64()-lastTime<TICKS_PER_FRAME){
        SDL_Delay(d);
    }
}
//list shuffilng
void Shuffle(std::vector<int> &v){
    srand(time(0));
    for(int i=0; i<v.size();i++){
        std::swap(v[i], v[rand()%v.size()]);
    }
}
//drawing list
void DrawList(std::vector<int> &v, std::pair<int, int> comp, int pivot, int pointer){
    
    //width of columns
    int a= WIDTH/LIST_SIZE;
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    for(int i=0; i<LIST_SIZE; i++){
        
        //default color
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        //comparing elemenst color
        if(i==comp.first||i==comp.second){
            SDL_SetRenderDrawColor(renderer, 100, 255, 100, 255);
        }
        //pivot color
        if(i==pivot){
            SDL_SetRenderDrawColor(renderer, 100, 100, 255, 255);
        }
        //pointer color
        if(i==pointer){
            SDL_SetRenderDrawColor(renderer, 200, 200, 255, 255);
        }
        //column
        int b=v[i]*HEIGHT/LIST_SIZE;
        SDL_Rect r;
        r.x = i*a;
        r.y = HEIGHT-b;
        r.w = a;
        r.h = b;
        SDL_RenderFillRect(renderer, &r);
    }
    
    SDL_RenderPresent(renderer);
    
}
void ShowInstructions(){
    SDL_RenderClear(renderer);
    SDL_Rect iRect;
    iRect.h = 120;
    iRect.w = 238;
    iRect.y = (HEIGHT-iRect.h)/2;
    iRect.x = (WIDTH-iRect.w)/2;
    SDL_RenderCopy(renderer, instructions, NULL, &iRect);
    SDL_RenderPresent(renderer);
}
//checking if array is sorted
void CheckSorted(std::vector<int> &v){
    for(int i=1; i<LIST_SIZE; i++){
        if(v[i-1]>=v[i]){
            std::cout<<"SORTING ERROR HAS OCCURED\n";
        }
        DrawList(v, {i-1, i},-1,-1);
        Delay(FRAME_DELAY);
    }
    DrawList(v, {-1, -1},-1,-1);
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
    ShowInstructions();
    

}
//insertion sort algorithm
void InsertionSort(std::vector<int> &v, int l, int r){
    for(int i = l+1; i<= r; i++){
        //taking leftmost variable
        int temp = v[i];
        int j = i-1;

        while(j>=l && v[j]>temp){
            //pushing the temp variable as low as we can
            DrawList(v, {j, i},-1,-1);
            Delay(FRAME_DELAY);
            
            v[j+1]=v[j];
            j--;
            
        }
        //setting the temp as low as we can
        v[j+1] = temp;
        DrawList(v, {j+1, i},-1,-1);
        Delay(FRAME_DELAY);
        
        
    }
}
//mergesort merging technique
void Merge(std::vector<int> &v, int l, int m, int r){
    //breaking provided part of the array into two parts
    //m is the breaking index
    int len1 = m - l + 1, len2 = r - m;
    std::vector<int> left(len1), right(len2);
    //filling the variables of both created arrays
    for(int i=0; i<len1; i++){
        left[i]=v[l+i];
    }
    for(int i=0; i<len2; i++){
        right[i]=v[m+1+i];
    }

    //merging the arrays using comparision
    //provides a sorted chunk
    int i=0, j=0, k=l;
    while(i<len1 && j<len2){
        DrawList(v, {i+l, m+1+j}, -1, -1);
        Delay(FRAME_DELAY);
        if(left[i]<=right[j]){
            v[k] = left[i];
            i++;
        }
        else{
            v[k]=right[j];
            j++;
        }
        k++;
        
    }

    //copying the remaining fragments of arrays
    while(i<len1){
        v[k]=left[i];
        k++;
        i++;
        DrawList(v, {-1, -1}, -1, -1);
        Delay(FRAME_DELAY);
    }
    while(j<len2){
        v[k]=right[j];
        k++;
        j++;
        DrawList(v, {-1, -1}, -1, -1);
        Delay(FRAME_DELAY);
    }


}
//TimSort sorting algorithm using insertionsort and mergesort merging technique
void TimSort(std::vector<int> &v, int n){
    //sorting runs of size atmost of 32
    //dividing the array into chunks of 32 or less sorted elements
    for(int i=0; i<n; i+=RUN){
        InsertionSort(v, i, __min((i+RUN-1), (n-1)));
    }
    //merging all the runs by increasing the merge radius 
    for(int size = RUN; size < n; size = 2*size){
        for(int left = 0; left<n; left += 2*size){
            //setting midpoint between two runs
            int mid = left + size -1;
            int right = __min((left+2*size-1), (n-1));
            if(mid<right){
                //merging 2 neighbouring runs
                Merge(v, left, mid, right);
            }
        }
    }
}
void MergeSort(std::vector<int> &v, int l, int r){
    if(l>=r){
        return;
    }
    int m = (l+r)/2;
    MergeSort(v, l, m);
    MergeSort(v, m+1, r);
    Merge(v, l,m,r);
}


//quicksort helper function
int Partition(std::vector<int> &v, int l, int r){
    //setting pivot and pointer
    int pivot = v[r];
    int pointer = l-1;
    for(int i=l; i<r; i++){
        DrawList(v, {i, r}, r, pointer);
        Delay(FRAME_DELAY);
        //moving smaller elements to the left
        if(v[i]<=pivot){
            pointer++;
            std::swap(v[pointer], v[i]);
            DrawList(v, {i, r}, r, pointer);
            Delay(FRAME_DELAY);
        }
    }
    //no more elements smaller than pivot - moving pivot to the left
    std::swap(v[pointer+1], v[r]);
    DrawList(v, {-1, r}, r, pointer);
    Delay(FRAME_DELAY);
    //providing the best place of partition
    return pointer+1;
}
//sorting function
void QuickSort(std::vector<int> &v, int l, int r){
    if(l<r){
        //setting up partition
        int pi = Partition(v, l, r);
        //partitioning the list - divide and conquer
        QuickSort(v, l, pi-1);
        QuickSort(v, pi+1, r);
    }  
}
//sorting function
void BubbleSort(std::vector<int> &v){
    for(int i=0; i<v.size();i++){
        for(int j=0; j<v.size()-1-i;j++){
            
            if(v[j]>=v[j+1]){
                std::swap(v[j], v[j+1]);
            }
            DrawList(v, {j, j+1}, -1, -1);
            Delay(FRAME_DELAY);
        }
    }
}
void PrintList(std::vector<int> &v){
    for(int i: v){
        std::cout<<i<<" ";
    }
    std::cout<<"\n";
}
std::vector<int> CreateList(int size){
    std::vector<int> v;
    for(int i=0; i<size; i++){
        v.push_back(i);
    }
    Shuffle(v);
    //PrintList(v);
    return v;
}
void ChoosingSort(){
    
    bool condition = true;
    while(condition){
        std::vector<int> list = CreateList(LIST_SIZE);
        ShowInstructions();
        while(!proceed){
        }
        switch(choice){
            case 'e':{
                condition = false;
                run = false;
                return;
            }
            case '0':{
                isSorting = true;
                QuickSort(list, 0, LIST_SIZE-1);
                break;
            }
            case '1':{
                isSorting = true;
                BubbleSort(list);
                break;
            }
            case '2':{
                isSorting = true;
                TimSort(list, LIST_SIZE);
                break;
            }
            case '3':{
                isSorting = true;
                InsertionSort(list,0 , LIST_SIZE-1);
                break;
            }
            case '4':{
                isSorting = true;
                MergeSort(list,0 , LIST_SIZE-1);
                break;
            }
            default:{
                std::cout<<choice<<"\n";
                Delay(1000);
                break;
            }
        }
        CheckSorted(list);
        isSorting = false;
        proceed = false;
    }
    

}
void WindowSetup(){
    SDL_SetMainReady();
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer);
    SDL_RenderSetScale(renderer , 1, 1);
    SDL_SetWindowTitle(window, windowTitle);
    instructions = IMG_LoadTexture(renderer, "Instructions.png");
}
void FrameUpdate(){
    while(run){
        
        SDL_Event event;
        if(SDL_PollEvent(&event)){
            if(event.type==SDL_KEYDOWN){
                if(event.key.keysym.sym==SDLK_ESCAPE){
                    break;
                }
                if(!isSorting){
                    switch(event.key.keysym.sym){
                        case SDLK_0:{
                            choice = '0';
                            proceed = true;
                            break;
                        }
                        case SDLK_1:{
                            choice = '1';
                            proceed = true;
                            break;
                        }
                        case SDLK_2:{
                            choice = '2';
                            proceed = true;
                            break;
                        }
                        case SDLK_3:{
                            choice = '3';
                            proceed = true;
                            break;
                        }
                        case SDLK_4:{
                            choice = '4';
                            proceed = true;
                            break;
                        }
                        case SDLK_e:{
                            choice = 'e';
                            proceed = true;
                            break;
                        }
                        default:{
                            proceed = true;
                            break;
                        }
                    }
                }
            }
            if(event.type == SDL_QUIT){
                break;
            }
        }
        Delay(FRAME_DELAY);
        lastTime = SDL_GetTicks64();
    }
}
int main(int argc, char*argv[]){
    WindowSetup();
    std::thread sorting(ChoosingSort);
    FrameUpdate();
    SDL_DestroyWindow(window);
    SDL_Quit();
    return EXIT_SUCCESS;
}

