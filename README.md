# QImagesWidget

## 简介



**QImagesWidget**是项目`mrscan2`的子项目。用于以多种方式显示图片，并支持在图片上绘制简单的图形。

项目定义了`constructor`接口，用于将数据转化为图片，比如将mrd文件转化为图片、将dicom文件转化为图片都需要定义不同的`constructor`





## 接口



### addSet(QList<QImage> images, QString setName)



一次扫描，一个线圈接收到的k空间的数据重建得到的图像称为`set`。



### addLine



### addRect()



### addCircle()



