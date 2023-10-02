#include "FormulaWidget.h"

FormulaWidget::FormulaWidget(QWidget *parent) :
    QDialog(parent),
    formulaLineEdit(new QLineEdit(this))
{
    QLabel *introductoryLabel = new QLabel(tr("This window is intended for entering mathematical formulas. You can use the following operators:\n"
                                              "  - Addition: +;\n"
                                              "  - Subtraction: -;\n"
                                              "  - Multiplication: *;\n"
                                              "  - Division: /;\n"
                                              "  - Degree: a^b (for example, 2^3 for 2 to the power of 3);\n"
                                              "  - Using trigonometric formulas (e.g. sin, cos, tan, con, sec, cosec);\n"
                                              "  - Root: sqrt(a) (for example, sqrt(16) for the square root of 16);\n"
                                              "  - Using the constant π instead of PI.\n"
                                              "Enter your formula:"), this);

    QPushButton *okButton = new QPushButton(tr("OK"), this);
    okButton->setIconSize(QSize(24, 24));
    okButton->setFixedSize(QSize(100, 30));

    connect(okButton, &QPushButton::clicked, this, &FormulaWidget::acceptFormula);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(introductoryLabel);
    layout->addWidget(formulaLineEdit);
    layout->addWidget(okButton);
    layout->setAlignment(okButton, Qt::AlignHCenter);

    setLayout(layout);

    setWindowIcon(QIcon(":/res/Icons-file/insert-formula-icon"));
    setWindowTitle(tr("Insert formula"));
}

FormulaWidget::~FormulaWidget(){}

QString FormulaWidget::insertFormula()
{
    formulaLineEdit->clear();
    if (exec() == Accepted) {
        QString formula = formulaLineEdit->text();
        return formatFormula(formula);
    }
    return QString();
}

void FormulaWidget::acceptFormula()
{
    accept();
}

QString FormulaWidget::formatFormula(const QString &inputFormula)
{
    QString formula = inputFormula;

    formula.replace("&", "&amp;");
    formula.replace("+", "&#43;");
    formula.replace("-", "&#8722;");
    formula.replace("*", "&#215;");
    formula.replace("PI", "π");

        formula.replace(QRegularExpression("([a-zA-Z0-9]+)\\(([^)]+)\\)\\^(\\d+)"), "\\1(\\2)<sup>\\3</sup>");
    formula.replace("sqrt", "<span>&radic;</span>");
    formula.replace(QRegularExpression("(\\w+)\\^(\\w+)"), "\\1<sup>\\2</sup>");
    formula.replace(QRegularExpression("([a-zA-Z0-9]+)\\s*/\\s*([a-zA-Z0-9]+)"),
                    "<span class=\"numerator\">\\1</span><span class=\"fraction-line\"></span><span class=\"denominator\">\\2</span>");

    return formula;
}
