#ifndef FAVORITEEDITDIALOG_H
#define FAVORITEEDITDIALOG_H

#include "ui_favoriteeditdialog.h"
class Settings;
class FavoriteEdit;

class FavoriteEditDialog : public QDialog, private Ui::FavoriteEditDialog
{
    Q_OBJECT
public:
    FavoriteEditDialog(Settings *settings, QWidget *parent = 0);
    virtual ~FavoriteEditDialog();

    FavoriteEdit *favoriteEdit() const { return m_favoriteEdit; };

public slots:
    void accept();

private:
    FavoriteEdit *m_favoriteEdit;
    Settings *m_settings;
};

#endif
