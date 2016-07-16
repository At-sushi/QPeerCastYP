#include "favoriteeditdialog.h"
#include "favoriteedit.h"
#include "settings.h"
#include "application.h"
#include "yellowpagemanager.h"

FavoriteEditDialog::FavoriteEditDialog(Settings *settings, QWidget *parent)
    : QDialog(parent), m_settings(settings)
{
    setupUi(this);

    connect(buttonBox, SIGNAL(accepted()), this, SLOT(on_accept()));
    // connect(buttonBox, SIGNAL(rejected()), this, SLOT(on_reject()));

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

void FavoriteEditDialog::on_accept()
{
    if (m_favoriteEdit->isDirty()) {
        m_favoriteEdit->write();
        qApp->yellowPageManager()->update();
    }
}

