# QImagesWidget
## Introduction

**QImagesWidget** is a subproject of the `mrscan2` project. Features include:

- Display multiple images in various ways
- Support drawing simple graphics on images
- Support image export
- Support handling mouse click events on images

## Interface

### init(size_t r, size_t c, size_t w, size_t h)

Initialize the widget with specified rows, columns, width, and height.

### setImages(QList<QImage> images)

Set the images to be displayed. The images will be arranged according to the current layout settings.

### Layout Configuration

- `setRowNum(size_t rows)` / `rowNum()`: Set/get the number of rows
- `setColNum(size_t cols)` / `colNum()`: Set/get the number of columns
- `setPageIndex(size_t index)` / `pageIndex()`: Set/get the current page index
- `setWidth(size_t width)` / `width()`: Set/get the width of each image 
- `setHeight(size_t height)` / `height()`: Set/get the height of each image

### Adding Graphics

- `addItem(int row, int col, QGraphicsItem* item)`: Add a graphics item to the image at specified row and column
- `addItem(int index, QGraphicsItem* item)`: Add a graphics item to the image at specified index

### Updating Display

- `updateMarkers()`: Update the graphics and markers displayed on images

### Signals

- `imageClicked(int row, int col, QPointF pos)`: Emitted when user clicks on an image. 
  - `row`: The row index of the clicked grid cell
  - `col`: The column index of the clicked grid cell
  - `pos`: The relative position of the click within the scene (in scene coordinates)