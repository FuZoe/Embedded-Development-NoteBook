我这里有一个Demo

[基于QML的可复用拖拽组件](https://github.com/FuZoe/component_loader)

下面我将对这个Demo进行讲解：

在这个Demo中，dragItem是左侧要拖动的小方块，DropArea是右侧放置区。dragItem有一只手(Drag)事件(在它的MouseArea)，用来抓住DropArea的位置。

我们来看看用到了哪些Drag事件：

    // 启动拖拽

    dragItem.Drag.mimeData = {
    
    "text/plain": button.text
        
    }
    dragItem.Drag.supportedActions = Qt.CopyAction //Qt.CopyAction的宏定义值为2
    
    dragItem.Drag.active = true

### `Drag.mimeData`: 货物的“内容”和“类型” 📦

你可以把 **`mimeData`** 理解成你正在拖拽的**“货物”的详细清单和类型标签**。

当你在现实中拖动一个文件时，操作系统不仅知道你移动了一个“文件”，它还知道这个文件是“文本文档”（`.txt`）还是“图片”（`.jpg`）。`mimeData` 在编程里就是做同样的事情。

* **`mime type`（类型标签）**：这是一种标准化的格式，用来描述数据的类型。比如，`"text/plain"` 代表纯文本，`"image/png"` 代表 PNG 格式的图片。
* **`data`（货物内容）**：这个部分就是实际的数据本身。如果类型是 `text/plain`，数据就是具体的文字内容；如果类型是 `image/png`，数据就是这张图片的二进制信息。

所以，`Drag.mimeData` 就像一个**装货物的包裹**，包裹上贴着标签（`mime type`），包裹里装着实际的货物（`data`）。当你把这个包裹拖到某个地方时，接收方（比如 `DropArea`）会先看标签，来判断自己能不能接收这件货物，然后再处理里面的内容。

### `Drag.supportedActions`: 你允许对货物做什么？🤝

**`supportedActions`** 就像你告诉收货方，你允许他们对你送来的“货物”进行哪些**操作**。

这就像快递员送件时，会问你：“这个包裹是让你**拿走**（移动），还是让你**复制一份**（拷贝），还是**只看一眼**（链接）？”

* `Qt.MoveAction`: 允许移动。意思是，收货方可以把这件货物从原来的地方“拿走”。
* `Qt.CopyAction`: 允许复制。意思是，收货方可以复制一份货物，而原来的货物还在原地。
* `Qt.LinkAction`: 允许链接。这个操作比较少见，通常指的是只创建一个指向原文件的快捷方式或引用。

**`supportedActions`** 就是你（作为拖拽发起者）给出的一个**“许可列表”**。当你拖动时，收货方会看到这个列表，然后根据自己的能力和意愿，从列表中选择一个操作。比如，你只允许 `MoveAction`（只能移动），但接收方只能执行 `CopyAction`（只能复制），那么这次拖拽就无法成功。反之，如果接收方支持你的某个许可，比如它也支持 `MoveAction`，那么拖拽成功后，它就会选择这个操作，并返回给你一个结果。

Note:Drag.supportedActions的值默认为7，7代表什么意思？

在 Qt QML 中，每一个“支持的操作”都有一个对应的数字：

Qt.MoveAction 的值是 1

Qt.CopyAction 的值是 2

Qt.LinkAction 的值是 4

当你将这些数字相加时，就代表你同时支持了多种操作：

1 + 2 = 3：代表同时支持 MoveAction 和 CopyAction。

1 + 4 = 5：代表同时支持 MoveAction 和 LinkAction。

2 + 4 = 6：代表同时支持 CopyAction 和 LinkAction。

1 + 2 + 4 = 7：代表同时支持 MoveAction、CopyAction 和 LinkAction。

所以，7意味着允许接收方进行移动、复制和链接这三种操作。

这两个属性是**拖拽交互的核心**。一个定义了**拖什么**（内容和类型），另一个定义了**能干什么**（操作许可）。 
