
将工程修改为一个通用的安卓音频播放器。
UI界面组件及其布局：
顶部音频信息区：
音频封面：显示当前播放音频的封面图片。
音频标题：显示音频的标题。
音频艺术家：显示音频的艺术家名称。
底部播放控制区：
进度条：显示音频播放进度，并允许用户拖动以跳转到音频的特定部分。进度条最左侧显示当前进度时间，最右侧显示总时间。
然后下面从左往右依次是：
1.循环/随机播放按钮：控制音频播放的循环模式（单曲循环、列表循环、随机播放）。
2. 上一曲：用于切换到上一首
3. 播放/暂停按钮：控制音频的播放和暂停状态。
4. 下一曲按钮：用于切换到下一首音频。
5. 播放列表：显示当前播放列表中的所有音频条目，用户可以点击条目切换播放
界面细节需求：
五个控制按钮等高等宽等间距，使用自定义图标
控制区距离底部有合适的距离
播放列表中当前正在播放的文件名高亮显示
代码需求：
kotlin语言
模块化设计，将不同功能的代码写在单独的文件中，说明具体文件路径。尽可能的细化模块，避免单文件过长，提高代码的可维护性
使用audiotrack，使播放的音频文件数据能够经过C语言算法处理后再输出
聊天用中文；你生成的代码块用全英文，包括其中的注释；
封装一个logger，有不同的等级，主要是提供代码所在的文件：行数 @方法名
实现细节：
首次启动APP时，扫描音乐文件，将文件信息保存到数据库中，并显示在播放列表中。此后如果用户不手动扫描，则使用数据库中保存的信息。
