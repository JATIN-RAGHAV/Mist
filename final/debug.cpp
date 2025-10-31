#include <cctype>
#include <vector>
#include <stdio.h>
void print_hex(std::vector<uint8_t>&v){
        int n = v.size();
        for(int i = 0;i<n;i++){
                if(std::isalpha(v[i])){
                        putchar(v[i]);
                }
                else{
                        printf("%x",v[i]);
                }
        }
        putchar('\n');
}
