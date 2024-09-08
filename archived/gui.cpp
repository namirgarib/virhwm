#include <QApplication>
#include <QLineEdit>
#include <QPushButton>
#include <QGridLayout>
#include <QWidget>
#include <QLabel>
#include <QMessageBox>
#include <QTextEdit>
#include <QKeyEvent>

short memory[64];

int gui(int argc, char **argv) {
    QApplication app(argc, argv);

    QWidget window;
    QGridLayout layout(&window);
    // Add column labels
    for (int i = 0; i < 8; i++) {
        QLabel *label = new QLabel(QString::number(i), &window);
        layout.addWidget(label, 0, i + 1);
    }

    // Add row labels
    for (int i = 0; i < 8; i++) {
        QLabel *label = new QLabel(QString(QChar('A' + i)), &window);
        layout.addWidget(label, i + 1, 0);
    }

    QLineEdit *fields[64];
    for (int i = 0; i < 64; i++) {
        fields[i] = new QLineEdit(&window);
        fields[i]->setFixedSize(40, 20);  // Set the size of the fields (40x40 pixels)
        layout.addWidget(fields[i], i / 8 + 1, i % 8 + 1);  // Arrange in 8x8 grid
    }

    QPushButton button("Write", &window);
    layout.addWidget(&button, 11, 4, 1, 2);  // Span the button across the bottom row

    QObject::connect(&button, &QPushButton::clicked, [&]() {
        for (int i = 0; i < 64; i++) {
            QString text = fields[i]->text();
            memory[i] = text.isEmpty() ? 0 : text.toInt();
        }

        QString memoryText = "<h2>Memory content:</h2><table>";
        for (int i = 0; i < 64; i++) {
            if (i % 8 == 0) memoryText += "<tr>";
            memoryText += "<td style='padding: 0 20px;'>" + QString::number(memory[i]) + "</td>";
            if (i % 8 == 7) memoryText += "</tr>";
        }

        memoryText += "</table>";
        QDialog dialog;
        QTextEdit textEdit;
        textEdit.setHtml(memoryText);
        textEdit.setWordWrapMode(QTextOption::NoWrap);
        QVBoxLayout layout;
        layout.addWidget(&textEdit);

        QPushButton closeButton("START");
        QObject::connect(&closeButton, &QPushButton::clicked, QCoreApplication::instance(), &QCoreApplication::quit);
        layout.addWidget(&closeButton);
        dialog.setLayout(&layout);
        dialog.resize(560, 315);
        dialog.exec();
    });

    window.show();

    return app.exec();
}