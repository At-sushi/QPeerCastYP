#include "favoriteeditdialog.h"
#include "favoriteedit.h"
#include "settings.h"
#include "application.h"
#include "yellowpagemanager.h"

FavoriteEditDialog::FavoriteEditDialog(Settings *settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    setupUi(this);

    m_favoriteEdit = new FavoriteEdit(settings, this);
    m_favoriteEdit->setValue();
    verticalLayout->insertWidget(0, m_favoriteEdit);

    setModal(true);
    show();
}

FavoriteEditDialog::~FavoriteEditDialog()
{
    delete m_favoriteEdit;
}

void FavoriteEditDialog::accept()
{
    if (m_favoriteEdit->isDirty()) {
        m_favoriteEdit->write();
        qApp->yellowPageManager()->update();
    }
    QDialog::accept();
}

