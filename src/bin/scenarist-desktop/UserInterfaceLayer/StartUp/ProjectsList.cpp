#include "ProjectsList.h"

#include "ProjectFileWidget.h"

#include <QAbstractItemModel>
#include <QVBoxLayout>

using UserInterface::ProjectsList;
using UserInterface::ProjectFileWidget;


ProjectsList::ProjectsList(QWidget* _parent) :
	QScrollArea(_parent),
	m_model(nullptr)
{
	initView();
}

void ProjectsList::setModel(QAbstractItemModel* _model, bool _isRemote)
{
	if (m_model != _model) {
		QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(widget()->layout());

		//
		// Стираем старый список проектов
		//
		while (layout->count() > 0) {
			QLayoutItem* item = layout->takeAt(0);
			if (item && item->widget()) {
				item->widget()->hide();
				item->widget()->deleteLater();
			}
		}

		//
		// Сохраняем новую модель
		//
		m_model = _model;

		//
		// Строим новый список проектов
		//
		if (m_model != nullptr) {
			for (int row = 0; row < m_model->rowCount(); ++row) {
				const QModelIndex projectIndex = m_model->index(row, 0);
				const QString projectName = projectIndex.data(Qt::DisplayRole).toString();
				const QString projectPath = projectIndex.data(Qt::WhatsThisRole).toString();
				const QStringList users = projectIndex.data(Qt::UserRole).toStringList();

				ProjectFileWidget* project = new ProjectFileWidget;
				project->setProjectName(projectName);
				project->setFilePath(projectPath);
				project->setIsRemote(_isRemote);
				for (const QString& user : users) {
					if (!user.simplified().isEmpty()) {
						const QStringList userInfo = user.split(";");
						project->addCollaborator(userInfo.value(0), userInfo.value(1), userInfo.value(2));
					}
				}
				//
				connect(project, &ProjectFileWidget::clicked, this, &ProjectsList::handleProjectClick);
				connect(project, &ProjectFileWidget::removeUserRequested, this, &ProjectsList::handleRemoveUserRequest);

				layout->addWidget(project);
			}

			layout->addStretch(1);
		}
	}
}

QAbstractItemModel* ProjectsList::model() const
{
	return m_model;
}

void ProjectsList::handleProjectClick()
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(widget()->layout());
		const int row = layout->indexOf(project);
		emit clicked(m_model->index(row, 0));
	}
}

void ProjectsList::handleRemoveUserRequest(const QString& _email)
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(widget()->layout());
		const int row = layout->indexOf(project);
		emit removeUserRequested(m_model->index(row, 0), _email);
	}
}

void ProjectsList::initView()
{
	QVBoxLayout* layout = new QVBoxLayout;
	layout->setContentsMargins(QMargins());
	layout->setSpacing(0);
	QWidget* content = new QWidget;
	content->setLayout(layout);

	setWidget(content);
	setWidgetResizable(true);
}
