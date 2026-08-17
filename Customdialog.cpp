#include "Customdialog.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

CustomDialog::CustomDialog(const QString &message, QWidget *parent)
    : QDialog(parent)
{
    setupUI(message);
}

void CustomDialog::setupUI(const QString &message)
{
    setWindowTitle(tr("Confirm"));
    setModal(true);
    setMinimumWidth(400);
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setStyleSheet(QStringLiteral(
        "QDialog {"
        "  background-color: #14181e;"
        "  color: #eef1f4;"
        "}"
        "QLabel {"
        "  color: #c8ced6;"
        "  padding: 4px 2px;"
        "}"
        "QPushButton {"
        "  background-color: #1c222a;"
        "  color: #e6eaef;"
        "  border: 1px solid #3a424c;"
        "  border-radius: 4px;"
        "  padding: 8px 18px;"
        "  min-width: 96px;"
        "  min-height: 34px;"
        "}"
        "QPushButton:hover {"
        "  background-color: #262d36;"
        "}"
        "QPushButton#continueBtn {"
        "  background-color: #6ba3b5;"
        "  color: #0b1014;"
        "  border: 1px solid #6ba3b5;"
        "}"
        "QPushButton#continueBtn:hover {"
        "  background-color: #7db3c4;"
        "}"
        "QPushButton#cancelBtn {"
        "  background: transparent;"
        "}"
    ));

    auto *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(24, 22, 24, 18);
    mainLayout->setSpacing(18);

    auto *label = new QLabel(message, this);
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *continueButton = new QPushButton(tr("Continue"), this);
    auto *cancelButton = new QPushButton(tr("Cancel"), this);
    continueButton->setObjectName(QStringLiteral("continueBtn"));
    cancelButton->setObjectName(QStringLiteral("cancelBtn"));
    continueButton->setDefault(true);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    buttonLayout->addStretch();
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(continueButton);

    mainLayout->addWidget(label);
    mainLayout->addLayout(buttonLayout);

    connect(continueButton, &QPushButton::clicked, this, &CustomDialog::onContinueClicked);
    connect(cancelButton, &QPushButton::clicked, this, &CustomDialog::onCancelClicked);

    adjustSize();
}

bool CustomDialog::showCustomDialog(const QString &message, QWidget *parent)
{
    CustomDialog dialog(message, parent);
    return dialog.exec() == QDialog::Accepted;
}

void CustomDialog::onContinueClicked()
{
    accept();
}

void CustomDialog::onCancelClicked()
{
    reject();
}
