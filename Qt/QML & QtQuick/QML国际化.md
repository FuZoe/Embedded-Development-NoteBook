我们以Main.qml的翻译为例

首先：

      lupdate Main.qml -ts translations/myapp_zh_CN.ts #生成待翻译文件

      linguist translations/myapp_zh_CN.ts #翻译官打开文件，手动翻译后，点击保存

      lrelease translations/myapp_zh_CN.ts -qm translations/myapp_zh_CN.qm #将翻译后的文件转为二进制


其中翻译官翻译界面如图所示
<img width="1494" height="1230" alt="image" src="https://github.com/user-attachments/assets/52e1327a-0e7d-4b33-9731-3f586b4eab43" />


然后将translations/myapp_zh_CN.qm手动放到构建目录下（例如我使用命令行工具放置）
<img width="1718" height="912" alt="image" src="https://github.com/user-attachments/assets/4bfe2c55-4790-4d00-ad3a-faa1f20da965" />


然后把（默认的）主函数文件改为如下：

      #include <QGuiApplication>
      #include <QQmlApplicationEngine>
      #include <QTranslator>
      #include <QLocale>
      
      int main(int argc, char *argv[])
      {
          QGuiApplication app(argc, argv);
      
          // 创建翻译器
          QTranslator translator;
      
          // 获取系统语言环境
          QLocale locale = QLocale::system();
          QString languageCode = locale.name(); // 例如：en_EN, zh_CN等
      
          // 加载对应的翻译文件
          QString translationFile = QString("translations/myapp_%1.qm").arg(languageCode);
      
          // 尝试加载翻译文件
          if (translator.load(translationFile)) {
              app.installTranslator(&translator);
              qDebug() << "翻译文件加载成功:" << translationFile;
          } else {
              qDebug() << "翻译文件加载失败:" << translationFile;
              // 如果系统语言的翻译文件不存在，尝试加载英文翻译
              if (translator.load("translations/myapp_en_EN.qm")) {
                  app.installTranslator(&translator);
                  qDebug() << "使用英文翻译文件";
              }
          }
      
          QQmlApplicationEngine engine;
          QObject::connect(
              &engine,
              &QQmlApplicationEngine::objectCreationFailed,
              &app,
              []() { QCoreApplication::exit(-1); },
              Qt::QueuedConnection);
          engine.loadFromModule("[!!!喂喂喂，这里替换为项目名]", "Main");
          return app.exec();
      }

编译运行，效果：
<img width="954" height="746" alt="image" src="https://github.com/user-attachments/assets/d8215212-b7c7-4e2e-abd5-c7f705e76bb4" />
