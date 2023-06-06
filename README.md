# SoftGrating_Lincac  
上一代软渲染优化和重构，更加符合OpenGL渲染流程  
# 效果  
![LX_)ZD05GOX~KW4$9RN%81C](https://github.com/Lincac/SoftGrating_Lincac/assets/83110022/53a6b1f6-6d61-4e76-878c-a2d802eb67ef)  
![6}{81JTKR CLE)4IYD}FV0](https://github.com/Lincac/SoftGrating_Lincac/assets/83110022/beb7e22b-0f93-4ba8-b99c-5a926ea6a45d)  
# 优化  
1.添加了相机（围绕固定点旋转，能维持15fps左右，模型较小的能达到20fps以上，鼠标滚轮改变相机fov，会影响帧率）   
2.添加了背面剔除，并提前至光栅化之前，能较大提升速度。  
3.采样差值，坐标差值  
4.齐次裁剪
# 待优化
shadowmap、pbr，deferred...


