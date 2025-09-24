
/* 一个QWizard编写的向导界面，CMakeLists.txt只需要包含本main.cpp文件即可
 * 官方文档：https://doc.qt.io/qt-6/zh/qwizard.html
 */
#include <QApplication>
#include <QWizard>
#include <QWizardPage>
#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>

// 自定义QWizardPage类，用于访问registerField方法
class RegistrationPage : public QWizardPage
{
    Q_OBJECT

public:
    RegistrationPage(QWidget *parent = nullptr) : QWizardPage(parent)
    {
        setTitle("Registration");
        setSubTitle("Please fill in your information.");

        QLabel *nameLabel = new QLabel("Name:");
        nameLineEdit = new QLineEdit;
        
        QLabel *emailLabel = new QLabel("Email:");
        emailLineEdit = new QLineEdit;

        QVBoxLayout *layout = new QVBoxLayout;
        layout->addWidget(nameLabel);
        layout->addWidget(nameLineEdit);
        layout->addWidget(emailLabel);
        layout->addWidget(emailLineEdit);
        setLayout(layout);

        // 注册字段，用于验证
        registerField("name*", nameLineEdit);
        registerField("email*", emailLineEdit);
    }

private:
    QLineEdit *nameLineEdit;
    QLineEdit *emailLineEdit;
};
QWizardPage *createIntroPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Introduction");

    QLabel *label = new QLabel("This wizard will help you register your copy "
                               "of Super Product Two.");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
}

QWizardPage *createRegistrationPage()
{
    return new RegistrationPage();
}

QWizardPage *createConclusionPage()
{
    QWizardPage *page = new QWizardPage;
    page->setTitle("Conclusion");
    page->setSubTitle("Registration completed successfully.");

    QLabel *label = new QLabel("You have successfully registered your copy of "
                               "Super Product Two. Thank you!");
    label->setWordWrap(true);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(label);
    page->setLayout(layout);

    return page;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

#ifndef QT_NO_TRANSLATION
    QString translatorFileName = QLatin1String("qtbase_");
    translatorFileName += QLocale::system().name();
    QTranslator *translator = new QTranslator(&app);
    if (translator->load(translatorFileName, QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        app.installTranslator(translator);
#endif

    QWizard wizard;
    wizard.addPage(createIntroPage());
    wizard.addPage(createRegistrationPage());
    wizard.addPage(createConclusionPage());

    wizard.setWindowTitle("Trivial Wizard");
    wizard.show();

    return app.exec();
}
#include "main.moc"
