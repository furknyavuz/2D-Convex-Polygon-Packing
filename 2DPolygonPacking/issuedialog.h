#ifndef ISSUEDIALOG_H
#define ISSUEDIALOG_H

#include <QDialog>

#include "ui_issuedialog.h"

namespace PolygonPacking
{

/**
 * @class IssueDialog
 *
 * @brief Dialog box class for show issues on the program
 *
 * @author Furkan Yavuz
 */
class IssueDialog : public QDialog
{
    Q_OBJECT
    
public:

    /**
     * Constructor sets the parent with given QWidget.
     * Takes title, definition and the solution of the issue.
     *
     * @param parent parent widget
     * @param title title of issue
     * @param issue definition of issue
     * @param solution solution of issue
     */
    explicit IssueDialog(QWidget *parent = 0, QString title = "", QString description = "", QString solution = "");

    /**
     * Default Destructor.
     */
    ~IssueDialog();
    
private slots:

    /**
     * Ok button handler.
     */
    void on_pushButton_clicked();

private:
    Ui::IssueDialog *ui;
};

}
#endif // ISSUEDIALOG_H
