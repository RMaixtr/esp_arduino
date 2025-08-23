#pragma once

#define IMU_LIMIT 32

template <typename T>
class calc_data
{
public:
    uint8_t index = 0;
    uint8_t is_max_new = false;
    uint8_t size = 0;
    bool is_full = false;
    T data[IMU_LIMIT];
    T max = -65535;
    T min = 65535;
    T abs_max = 0;
    T sum = 0;
    double sum2 = 0;
    T mean;
    T rms;

    calc_data(uint8_t s = IMU_LIMIT){
        size = s;
    }

    void add (T d){
        if (d > max) {max = d; is_max_new = true;}
        if (d < min) {min = d; is_max_new = false;}
        sum += d;
        sum2 += d * d;
        T tmp = data[index];
        data[index++] = d;
        if (is_full){
            sum -= tmp;
            sum2 -= tmp * tmp;
            mean = sum / size;
            rms = sqrt(sum2 / size);
            if (tmp == max){
                max = - 32767;
                for (size_t i = 0; i < size; i++){
                    if (data[i] > max) {max = data[i];}
                }
            }
            if (tmp == min){
                min = 32767;
                for (size_t i = 0; i < size; i++){
                    if (data[i] < min) {min = data[i];}
                }
            }
        }
        abs_max = abs(max) > abs(min) ? abs(max) : abs(min);
        if (index >= size){index = 0; is_full = true;};
        
    }
};
