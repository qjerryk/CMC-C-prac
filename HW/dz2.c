#include <stdio.h>
#include <stdlib.h>


unsigned char min(unsigned char a, unsigned char b){
    if (a > b) return b;
    return a;
}

unsigned char max(unsigned char a, unsigned char b){
    if (a > b) return a;
    return b;
}

int SignificantBitsCounter(unsigned char num){ 
    int cnt = 0;
    while (num){
        num >>= 1;  // cчитаем кол-во значащих бит в числе
        ++cnt;
    }
    return cnt;
}

int PackedArraySize(int n, unsigned char* array){
    int ind;
    int res = 1 + (n+1)/2; // на кол-во элементов в массиве + элементы, хранящие кол-во значащих бит
    int cnt = 0;
    for (ind = 0; ind < n; ++ind){
        cnt += SignificantBitsCounter(array[ind]); // считаем кол-во бит, необходимое для хранения значащих частей элементов 
    }
    return res + (cnt+7)/8;

}

void ReadArray(int n, unsigned char** array){
    int ind;
    for (ind = 0; ind < n; ind++){
        scanf("%hhd", (*array+ind));
    } 
}

void PrintBitArray(int n, unsigned char* array){
    int ind, tmp, mask;
    for (ind = 0; ind < n; ++ind){
        for (mask = 1 << (sizeof(unsigned char)*8 - 1), tmp = sizeof(unsigned char)*8 - 1 ; mask > 0; mask >>= 1, tmp--){
            printf("%d", (array[ind] & mask) >> (tmp)); //цикл для побитового вывода числа
        }
        printf(" ");
    }
}

unsigned char* toPacked(int n, unsigned char* array){
    int ind, tmp_ind, bit_ptr, array_size = PackedArraySize(n, array);
    unsigned char mask;
    unsigned char* PackedArray =(unsigned char*)calloc(array_size, sizeof(unsigned char));
    PackedArray[0] = n;
    for (ind = 0; ind < n; ++ind){
        if (ind % 2 == 0){
            PackedArray[1+(ind)/2] = SignificantBitsCounter(array[ind]);
        }else{
            PackedArray[1+(ind)/2] = (PackedArray[1+(ind)/2]<<(sizeof(unsigned char))*4) | SignificantBitsCounter(array[ind]);
        }
    }
    if (n & 1){
        PackedArray[(n+1)/2] <<= sizeof(unsigned char)*4; // дополняем нулями последний элемент массива в случае нечетного количества элементов
    }
    for (ind = (n+1)/2 + 1, bit_ptr = 8*sizeof(unsigned char); ind <= array_size;){ // заполняем оставшиеся биты значащими битами изначальных элементов
        for (tmp_ind = 0; tmp_ind < n; tmp_ind++){
            mask = (1 << SignificantBitsCounter(array[tmp_ind])) - 1;
            if (bit_ptr == 8*sizeof(unsigned char)){
                PackedArray[ind] = 0;
            }
            if (bit_ptr < SignificantBitsCounter(array[tmp_ind])){
                PackedArray[ind] = PackedArray[ind] | ((array[tmp_ind] & mask) >> (SignificantBitsCounter(array[tmp_ind]) - bit_ptr));
                ++ind;
                mask = (1 << (SignificantBitsCounter(array[tmp_ind]) - bit_ptr)) - 1;
                PackedArray[ind] |= array[tmp_ind]&mask;
                PackedArray[ind] <<= 8*sizeof(unsigned char) - SignificantBitsCounter(array[tmp_ind]) + bit_ptr;
                bit_ptr = 8*sizeof(unsigned char) - (SignificantBitsCounter(array[tmp_ind]) - bit_ptr);
            }else {
                PackedArray[ind] = PackedArray[ind] | ((array[tmp_ind] & mask) << (bit_ptr - SignificantBitsCounter(array[tmp_ind])));
                bit_ptr -= SignificantBitsCounter(array[tmp_ind]);
            }
            if (!bit_ptr){
                bit_ptr = 8*sizeof(unsigned char);
                ++ind;
            }
        } 
        break; 

    }
    PrintBitArray(array_size, PackedArray);
    return PackedArray;
}


int main(){
    int n, ind;
    scanf("%d", &n);
    unsigned char* array = (unsigned char*)calloc(n, sizeof(unsigned char));
    ReadArray(n, &array);
    unsigned char* PackedArray = toPacked(n, array);
    return 0;
}
