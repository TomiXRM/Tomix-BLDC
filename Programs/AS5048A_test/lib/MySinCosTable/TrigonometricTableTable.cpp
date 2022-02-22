#include "TrigonometricTable.h"

float sin32_T(int theta){
      float let;
      int theta_cal;
      //0~360の値に変換
      while (theta < 0) {
            theta += 360;
      }
      if(theta >= 360) {
            theta = theta%360;
      }

      theta_cal = theta%90;
      if(theta >= 90 && theta < 180) {
            theta_cal = 90 - theta_cal;
      }
      if(theta >=270 && theta < 360) {
            theta_cal = 90 - theta_cal;
      }

      let = _sin[theta_cal];

      if(theta >= 0 && theta <= 90) {
            //0~90 第一象限
      }else if(theta > 90 && theta <= 180) {
            //91~180 第二象限
      }else if(theta > 180 && theta <= 270) {
            //181~270 第三象限
            let = -let;
      }else if(theta > 270 && theta < 360) {
            //271~360 第四象限
            let = -let;
      }
      return let;
}

float cos32_T(int theta){
      return sin32_T(theta + 90);
}
