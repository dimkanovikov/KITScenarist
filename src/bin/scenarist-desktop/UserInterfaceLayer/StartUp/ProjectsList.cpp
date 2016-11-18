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
				const bool isOwner = projectIndex.data(Qt::UserRole + 1).toBool();

				ProjectFileWidget* project = new ProjectFileWidget;
				project->setProjectName(projectName);
				project->setFilePath(projectPath);
				project->configureOptions(_isRemote, isOwner);
				for (const QString& user : users) {
					if (!user.simplified().isEmpty()) {
						const QStringList userInfo = user.split(";");
						project->addCollaborator(userInfo.value(0), userInfo.value(1), userInfo.value(2), isOwner);
					}
				}
				//
				connect(project, &ProjectFileWidget::clicked, this, &ProjectsList::handleProjectClick);
				connect(project, &ProjectFileWidget::editClicked, this, &ProjectsList::handleEditClick);
				connect(project, &ProjectFileWidget::removeClicked, this, &ProjectsList::handleRemoveClick);
				connect(project, &ProjectFileWidget::hideClicked, this, &ProjectsList::handleHideClick);
				connect(project, &ProjectFileWidget::shareClicked, this, &ProjectsList::handleShareClick);
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

int ProjectsList::projectRow(UserInterface::ProjectFileWidget* _project) const
{
	QVBoxLayout* layout = dynamic_cast<QVBoxLayout*>(widget()->layout());
	return layout->indexOf(_project);
}

void ProjectsList::handleProjectClick()
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		emit clicked(m_model->index(projectRow(project), 0));
	}
}

void ProjectsList::handleEditClick()
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		emit editRequested(m_model->index(projectRow(project), 0));
	}
}

void ProjectsList::handleRemoveClick()
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		emit removeRequested(m_model->index(projectRow(project), 0));
	}
}

void ProjectsList::handleHideClick()
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		emit hideRequested(m_model->index(projectRow(project), 0));
	}
}

void ProjectsList::handleShareClick()
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		emit shareRequested(m_model->index(projectRow(project), 0));
	}
}

void ProjectsList::handleRemoveUserRequest(const QString& _email)
{
	if (ProjectFileWidget* project = qobject_cast<ProjectFileWidget*>(sender())) {
		emit unshareRequested(m_model->index(projectRow(project), 0), _email);
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
