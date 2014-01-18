#include "issuedialog.h"
#include "ui_issuedialog.h"

namespace PolygonPacking
{

IssueDialog::IssueDialog(QWidget *parent, QString title, QString description, QString solution) :
    QDialog(parent),
    ui(new Ui::IssueDialog)
{
    ui->setupUi(this);
    ui->titleLabel->setText(title);
    ui->descriptionLabel->setText("Description : " + description);
    ui->solutionLabel->setText("Solution : " + solution);
}

IssueDialog::~IssueDialog()
{
    delete ui;
}

void IssueDialog::on_pushButton_clicked()
{
    QDialog::accept();
}

}
