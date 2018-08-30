using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
class Solution {

    static void Main(String[] args) {
        int[][] arr = new int[6][];
        for(int arr_i = 0; arr_i < 6; arr_i++){
           string[] arr_temp = Console.ReadLine().Split(' ');
           arr[arr_i] = Array.ConvertAll(arr_temp,Int32.Parse);
        }

        int cur_sum, max_sum = -10000000; //INT_MIN;
        for (int r = 0; r < 6; ++r) {
            for (int c = 0; c < 6; ++c) {
                if (r < 3 && c < 3 ) {
                    cur_sum = arr[r][c]   + arr[r][c+1]   + arr[r][c+2]
                                          + arr[r+1][c+1] + 
                              arr[r+2][c] + arr[r+2][c+1] + arr[r+2][c+2];

                    if(cur_sum > max_sum) {
                        max_sum = cur_sum;
                    }
                }
            }
        }
        Console.WriteLine(max_sum);

    }

}


