#include "SecurityLab.h"
#include <QMessageBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>

SecurityLab::SecurityLab(SecurityLabModel* ip_model, QWidget* parent)
	: QMainWindow(parent), mp_model(ip_model)
{
	ui.setupUi(this);
	resetPage();
	ui.users_view->setModel(mp_model);
	_InitConnections();
}

void SecurityLab::resetPage()
{
	QSize window_size;
	switch (mp_model->GetCurrentPage())
	{
	case Page::LoginPage:
		ui.LoginPage->setVisible(true);
		ui.AdminPage->setVisible(false);
		ui.UserPage->setVisible(false);
		window_size = { 221, 128 };
		setWindowTitle("Login");
		break;
	case Page::AdminPage:
		ui.AdminPage->setVisible(true);
		ui.LoginPage->setVisible(false);
		ui.UserPage->setVisible(false);
		window_size = { 550, 250 };
		setWindowTitle("Admin");
		break;
	case Page::UserPage:
		ui.UserPage->setVisible(true);
		ui.AdminPage->setVisible(false);
		ui.LoginPage->setVisible(false);
		window_size = { 230, 90 };
		setWindowTitle("User");
		break;
	}
	this->resize(window_size);
	this->setMinimumSize(window_size);
	this->adjustSize();
}

void SecurityLab::_InitConnections()
{
	auto isConnected = QObject::connect(mp_model, &SecurityLabModel::LoginUsernameChanged, this, [&](const QString text)
		{
			ui.login_username_edt->setText(text);
			ui.login_sign_in_btn->setEnabled(!text.isEmpty());
		});

	isConnected = QObject::connect(mp_model, &SecurityLabModel::LoginPasswordChanged, this, [&](const QString text)
		{
			ui.login_password_edt->setText(text);
		});

	isConnected = QObject::connect(ui.login_username_edt, &QLineEdit::textChanged, mp_model, &SecurityLabModel::SetLoginUsername);
	isConnected = QObject::connect(ui.login_password_edt, &QLineEdit::textChanged, mp_model, &SecurityLabModel::SetLoginPassword);

	isConnected = QObject::connect(ui.login_sign_in_btn, &QAbstractButton::clicked, mp_model, &SecurityLabModel::Login);

	isConnected = QObject::connect(mp_model, &SecurityLabModel::PageChanged, this, [&]()
		{
			resetPage();
		});

	isConnected = QObject::connect(ui.adm_logout_btn, &QAbstractButton::clicked, mp_model, &SecurityLabModel::Logout);
	isConnected = QObject::connect(ui.user_logout_btn_2, &QAbstractButton::clicked, mp_model, &SecurityLabModel::Logout);

	isConnected = QObject::connect(mp_model, &SecurityLabModel::Message, this, [&](QString message)
		{
			QMessageBox msg;
			msg.setText(message);
			msg.exec();
		});

	isConnected = QObject::connect(ui.adm_add_user_btn, &QAbstractButton::clicked, mp_model, [&]()
		{
			QDialog dlg(this);
			dlg.setWindowTitle(tr("New user"));

			QLineEdit* username = new QLineEdit(&dlg);

			QDialogButtonBox* btn_box = new QDialogButtonBox(&dlg);
			btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

			connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
			connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

			QFormLayout* layout = new QFormLayout();
			layout->addRow(tr("Username:"), username);
			layout->addWidget(btn_box);

			dlg.setLayout(layout);
			if (dlg.exec() == QDialog::Accepted) {
				mp_model->AddUser(username->text());
			}
		});

	isConnected = QObject::connect(ui.users_view, &QTableView::clicked, mp_model, &SecurityLabModel::SetIndex);

	isConnected = QObject::connect(mp_model, &SecurityLabModel::IndexChanged, this, [&](bool i_is_valid) {
		ui.adm_reset_verification_btn->setEnabled(i_is_valid);
		ui.adm_reset_verification_btn->setEnabled(i_is_valid);
		});

	isConnected = QObject::connect(ui.adm_block_btn, &QPushButton::clicked, mp_model, &SecurityLabModel::BlockUser);
	isConnected = QObject::connect(ui.adm_reset_verification_btn, &QPushButton::clicked, mp_model, &SecurityLabModel::DisableVerification);

	const auto ask_password = [](QWidget* parent, SecurityLabModel* ip_model)
	{
		QDialog dlg(parent);
		dlg.setWindowTitle("Reset password");

		QLineEdit* old_password = new QLineEdit(&dlg);
		QLineEdit* new_password = new QLineEdit(&dlg);

		QDialogButtonBox* btn_box = new QDialogButtonBox(&dlg);
		btn_box->setStandardButtons(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

		QObject::connect(btn_box, &QDialogButtonBox::accepted, &dlg, &QDialog::accept);
		QObject::connect(btn_box, &QDialogButtonBox::rejected, &dlg, &QDialog::reject);

		QFormLayout* layout = new QFormLayout();
		layout->addRow("old password:", old_password);
		layout->addRow("new password:", new_password);
		layout->addWidget(btn_box);

		dlg.setLayout(layout);
		if (dlg.exec() == QDialog::Accepted) {
			ip_model->ResetPassword(old_password->text(), new_password->text());
		}
	};

	isConnected = QObject::connect(ui.user_reset_password_btn_2, &QPushButton::clicked, mp_model, [&]()
		{
			ask_password(this, mp_model);
		});

	isConnected = QObject::connect(ui.adm_reset_password, &QPushButton::clicked, mp_model, [&]()
		{
			ask_password(this, mp_model);
		});

	isConnected = QObject::connect(ui.adm_info_btn, &QPushButton::clicked, mp_model, [&]()
		{
			QMessageBox msg;
			msg.setText(
				"Цей застосунок розроблено\nСтудентом БС-93 Луцюком Максимом(8 варіант)\n8.Наявність рядкових і прописних букв а також цифр"
			);
			msg.exec();
		});

}

SecurityLab::~SecurityLab()
{}
