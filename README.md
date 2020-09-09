# motion-alert-send-image-telegram
ESP32_Cam_Motion_Alert_Send_Image_to_Telegram

this project is for hobby project.

the device is ESP32-CAM with PIR sensor and Buzzer

PIR detect motion, if motion detected the camera capture and send to the telegram bot in the meantime the ESP32-CAM turn the buzzer on using PWM on channel 4.
I use FREERTOS to run the PWM buzer so the sending telegram and buzzer done by ESP32-CAM at the sametime.

You can learn this project for education only.

