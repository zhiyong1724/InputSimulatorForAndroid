# InputSimulator
一个运行json格式脚本的输入模拟器，依赖系统无障碍服务提供的输入模拟功能，在高版本系统中需要在电池管理设置中允许应用在后台运行，app默认执行在Download文件夹中的start.json文件。
## demo
``` json
[
    {
        "cmd": "echo",
        "text": "text"
    },
    {
        "cmd": "delay",
        "duration": 20000,
        "random": 0
    },
    {
        "cmd": "for",
        "body":[
        {
            "cmd": "echo",
            "text": "text"
        }
        ]
    }
]
```
## 语法
最外层一定是一个数组，数组里面是一个一个对象，对象一定要有cmd字段，一个对象是一个命令，不同的命令有不同的参数字段，模拟器会按顺序执行这些命令。
## 命令
|命令|参数|类型|描述|
|:-:|:-:|:-:|:-|
|echo|text|string|打印内容|
|delay|duration|number|延时时间，单位为毫秒|
|-|random|number|可选项，duration加上一个0到random范围的随机数|
|for|count|number|循环次数|
|-|body|array|循环体，数组里面可添加命令对象|
|touch|x|number|点击屏幕横坐标|
|-|y|number|点击屏幕纵坐标|
|-|duration|number|点击持续时间，单位为毫秒|
|-|random|number|可选项，x和y加上一个0到random范围的随机数|
|swipe|x|number|滑动起点屏幕横坐标|
|-|y|number|滑动起点屏幕纵坐标|
|-|end_x|number|滑动终点屏幕横坐标|
|-|end_y|number|滑动终点屏幕纵坐标|
|-|duration|number|点击持续时间，单位为毫秒|
|-|random|number|可选项，duration加上一个0到random范围的随机数|
## 代码仓库
https://github.com/zhiyong1724/InputSimulatorForAndroid.git
## BUG反馈
172471067@qq.com