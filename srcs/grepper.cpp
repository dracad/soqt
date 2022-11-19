#include "grepper.h"

grepper::grepper(QWidget *parent)
    : QWidget{parent}
{
    enable = false;
    inverse_match = false;
    caseinsensitive = false;
    match_line = false;

    auto grep_layout = new QVBoxLayout();
    auto selection   = new QGridLayout();
    auto box         = new QGroupBox("Filtering", this);


    auto enable_chk             = new QCheckBox(this);
    auto inverse_chk            = new QCheckBox(this);
    auto caseInsensitive_chk    = new QCheckBox(this);
    auto match_line_chk         = new QCheckBox(this);

    auto re_expression_in       = new QLineEdit(this);
    auto lbl                    = new QLabel(this);

    lbl->setText("Pattern");

    enable_chk->setText("Enable Grep");
    inverse_chk->setText("Inverse Match");
    caseInsensitive_chk->setText("Case Insensitive");
    match_line_chk->setText("Match whole line");

    selection->addWidget(enable_chk, 0, 0);
    selection->addWidget(inverse_chk, 0, 1);
    selection->addWidget(caseInsensitive_chk, 1, 0);
    selection->addWidget(match_line_chk, 1, 1);

    grep_layout->addWidget(box);
    grep_layout->addWidget(lbl);
    grep_layout->addWidget(re_expression_in);

    this->setLayout(grep_layout);
    box->setLayout(selection);

    connect(enable_chk, &QCheckBox::toggled, this, [this](bool checked){enable = checked; emit changed();});
    connect(inverse_chk, &QCheckBox::toggled, this, [this](bool checked){inverse_match = checked; emit changed();});
    connect(caseInsensitive_chk, &QCheckBox::toggled, this, [this](bool checked){caseinsensitive = checked; emit changed();});
    connect(match_line_chk, &QCheckBox::toggled, this, [this](bool checked){match_line = checked; emit changed();});
    connect(re_expression_in, &QLineEdit::textChanged, this, [this](const QString &text){pattern = text; emit changed();});

}

bool grepper::is_valid_text(const QString &text)
{
    if(!enable){
        return true;
    }

    QRegularExpression::PatternOption case_sensitivity = QRegularExpression::NoPatternOption;

    if(caseinsensitive){
        case_sensitivity = QRegularExpression::CaseInsensitiveOption;
    }

    QRegularExpression re(pattern, case_sensitivity);
    auto match = re.match(text);

    if(match.hasMatch() && !inverse_match){
       if(match_line){
           return match.captured(0).compare(text) == 0;
       }else{
           return true;
       }
       return true;
    }else if(!match.hasMatch() && inverse_match){
       return true;
    }

    return false;
}
