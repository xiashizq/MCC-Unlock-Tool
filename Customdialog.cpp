#include "Customdialog.h"

#include <QEvent>
#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QVBoxLayout>

CustomDialog::CustomDialog(Kind kind, const QString &message, QWidget *parent)
    : QDialog(parent)
{
    setupUI(kind, message);
}

void CustomDialog::setupUI(Kind kind, const QString &message)
{
    setModal(true);
    setWindowModality(Qt::WindowModal);
    setFixedWidth(420);
    setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground, true);

    const bool isConfirm = (kind == Kind::Confirm);
    setWindowTitle(isConfirm ? tr("Confirm") : tr("Notice"));

    setStyleSheet(QStringLiteral(
        "QDialog {"
        "  background-color: #14181e;"
        "  color: #eef1f4;"
        "  border: 1px solid #3a424c;"
        "}"
        "QWidget#titleBar {"
        "  background-color: #101318;"
        "  border: none;"
        "  border-bottom: 1px solid #2a3038;"
        "}"
        "QLabel#titleLabel {"
        "  color: #f4f6f8;"
        "  font-weight: 600;"
        "  padding: 0px;"
        "}"
        "QLabel#messageLabel {"
        "  color: #e6eaef;"
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
        "  border-color: #4b5562;"
        "}"
        "QPushButton:pressed {"
        "  background-color: #161b21;"
        "}"
        "QPushButton#continueBtn {"
        "  background-color: #6ba3b5;"
        "  color: #0b1014;"
        "  border: 1px solid #6ba3b5;"
        "}"
        "QPushButton#continueBtn:hover {"
        "  background-color: #7db3c4;"
        "  border-color: #7db3c4;"
        "}"
        "QPushButton#continueBtn:pressed {"
        "  background-color: #5a8fa0;"
        "}"
        "QPushButton#cancelBtn {"
        "  background: transparent;"
        "}"
        "QPushButton#closeBtn {"
        "  background: transparent;"
        "  border: none;"
        "  border-radius: 3px;"
        "  color: #8a929c;"
        "  min-width: 28px;"
        "  max-width: 28px;"
        "  min-height: 28px;"
        "  max-height: 28px;"
        "  padding: 0px;"
        "  font-size: 16px;"
        "}"
        "QPushButton#closeBtn:hover {"
        "  background-color: #3a2226;"
        "  color: #f0d0d4;"
        "}"
        "QFrame#accentBar {"
        "  background-color: #6ba3b5;"
        "  border: none;"
        "  max-height: 2px;"
        "  min-height: 2px;"
        "}"
        "QFrame#accentBarWarn {"
        "  background-color: #c4a15a;"
        "  border: none;"
        "  max-height: 2px;"
        "  min-height: 2px;"
        "}"
    ));

    auto *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(1, 1, 1, 1);
    rootLayout->setSpacing(0);

    auto *accentBar = new QFrame(this);
    accentBar->setObjectName(kind == Kind::Warning
                                 ? QStringLiteral("accentBarWarn")
                                 : QStringLiteral("accentBar"));

    m_titleBar = new QWidget(this);
    m_titleBar->setObjectName(QStringLiteral("titleBar"));
    m_titleBar->setFixedHeight(40);
    m_titleBar->installEventFilter(this);

    auto *titleLayout = new QHBoxLayout(m_titleBar);
    titleLayout->setContentsMargins(16, 0, 8, 0);
    titleLayout->setSpacing(8);

    auto *titleLabel = new QLabel(windowTitle(), m_titleBar);
    titleLabel->setObjectName(QStringLiteral("titleLabel"));
    titleLabel->setAttribute(Qt::WA_TransparentForMouseEvents, true);

    auto *closeButton = new QPushButton(QStringLiteral("×"), m_titleBar);
    closeButton->setObjectName(QStringLiteral("closeBtn"));
    closeButton->setCursor(Qt::PointingHandCursor);
    closeButton->setFocusPolicy(Qt::NoFocus);
    closeButton->setToolTip(tr("Close"));

    titleLayout->addWidget(titleLabel);
    titleLayout->addStretch();
    titleLayout->addWidget(closeButton);

    auto *body = new QWidget(this);
    auto *bodyLayout = new QVBoxLayout(body);
    bodyLayout->setContentsMargins(24, 20, 24, 18);
    bodyLayout->setSpacing(18);

    auto *label = new QLabel(message, body);
    label->setObjectName(QStringLiteral("messageLabel"));
    label->setWordWrap(true);
    label->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

    auto *buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(8);
    buttonLayout->addStretch();

    if (isConfirm) {
        auto *cancelButton = new QPushButton(tr("Cancel"), body);
        auto *continueButton = new QPushButton(tr("Continue"), body);
        cancelButton->setObjectName(QStringLiteral("cancelBtn"));
        continueButton->setObjectName(QStringLiteral("continueBtn"));
        continueButton->setDefault(true);
        buttonLayout->addWidget(cancelButton);
        buttonLayout->addWidget(continueButton);
        connect(continueButton, &QPushButton::clicked, this, &CustomDialog::onContinueClicked);
        connect(cancelButton, &QPushButton::clicked, this, &CustomDialog::onCancelClicked);
        connect(closeButton, &QPushButton::clicked, this, &CustomDialog::onCancelClicked);
    } else {
        auto *okButton = new QPushButton(tr("OK"), body);
        okButton->setObjectName(QStringLiteral("continueBtn"));
        okButton->setDefault(true);
        buttonLayout->addWidget(okButton);
        connect(okButton, &QPushButton::clicked, this, &CustomDialog::onContinueClicked);
        connect(closeButton, &QPushButton::clicked, this, &CustomDialog::onContinueClicked);
    }

    bodyLayout->addWidget(label);
    bodyLayout->addLayout(buttonLayout);

    rootLayout->addWidget(accentBar);
    rootLayout->addWidget(m_titleBar);
    rootLayout->addWidget(body);

    adjustSize();
}

bool CustomDialog::showCustomDialog(const QString &message, QWidget *parent)
{
    CustomDialog dialog(Kind::Confirm, message, parent);
    return dialog.exec() == QDialog::Accepted;
}

void CustomDialog::showInformation(const QString &message, QWidget *parent)
{
    CustomDialog dialog(Kind::Information, message, parent);
    dialog.exec();
}

void CustomDialog::showWarning(const QString &message, QWidget *parent)
{
    CustomDialog dialog(Kind::Warning, message, parent);
    dialog.exec();
}

void CustomDialog::onContinueClicked()
{
    accept();
}

void CustomDialog::onCancelClicked()
{
    reject();
}

bool CustomDialog::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == m_titleBar) {
        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            const auto *mouse = static_cast<QMouseEvent *>(event);
            if (mouse->button() == Qt::LeftButton) {
                m_dragging = true;
                m_dragOffset = mouse->globalPos() - frameGeometry().topLeft();
                return true;
            }
            break;
        }
        case QEvent::MouseMove: {
            if (m_dragging) {
                const auto *mouse = static_cast<QMouseEvent *>(event);
                move(mouse->globalPos() - m_dragOffset);
                return true;
            }
            break;
        }
        case QEvent::MouseButtonRelease: {
            const auto *mouse = static_cast<QMouseEvent *>(event);
            if (mouse->button() == Qt::LeftButton) {
                m_dragging = false;
                return true;
            }
            break;
        }
        default:
            break;
        }
    }
    return QDialog::eventFilter(watched, event);
}
