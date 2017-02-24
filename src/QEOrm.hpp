/**
 * @section ORM_DESCRIPTION ORM General Description 
 * QE ORM is a Object-relational mapping (ORM, O/RM, or O/R mapping tool) that allows you to map
 * your QObject subclasses to a SQL database.
 * It uses Qt Property system to identified what properties of an object should be mapped to specific 
 * database table columns.
 * It also uses QEAnnotation library to add customizations to those clases or properties and fit them 
 * to database constraints like primary keys, unique, or not null.
 * Let's see an simple example, a 'user' table that we will use in authentication process:
 * @code{.cpp}
 * // DbUser.hpp
 * #include <QObject>
 * class DBUser : public QObject
 * {
 * 	Q_OBJECT
 * 	Q_PROPERTY( int id MEMBER id)
 * 	Q_PROPERTY( QString name MEMBER name)
 * 
 * 	Q_CLASSINFO( "class", "@QE.ORM.TABLE=myUser @QE.ORM.INDEX=id")
 * 	Q_CLASSINFO( "id", "@QE.ORM.AUTO_INCREMENT=true")
 * 	Q_CLASSINFO( "name", "@QE.ORM.NULL=false @QE.ORM.MAX_LENGTH=255 @QE.ORM.UNIQUE=true)
 * 	public:
 * 		DBUser( QObject *parent = nullptr);
 * 	
 * 		int id;
 * 		QString name;
 * };
 * @endcode
 * In the above code, we have used the @c Q_CLASSINFO to add the following annotations:
 *  # 
 * 
 * @subsection ORM_CLASS_ANNOTATION ORM Class Annotation
 * Those are annotation which are refered to the class. All of them MUST USE @c "class" as key for @c Q_CLASSINFO :
 *  - @c @QE.ORM.TABLE : It is a string contains the database table name used to store object of this class. 
 * 	 By default, QE ORM uses the class name as a database table name.
 *  - @c @QE.ORM.TEMPORARY_TABLE : It creates the database table as a temporary one. By default, it is @c false.
 *  - @c @QE.ORM.EXPORT_PARENT : If it is @c true, it will also export properties from parents classes. By default, it is @c false.
 *  - @c @QE.ORM.PRIMARY_KEY : It is a comma-separated list of properties which will be used as a primary key. By
 * 	default, no primary key is defined unless an @c auto_increment field is found.
 * 
 * @subsection ORM_COLUMN_ANNOTATION ORM Column Annotation
 * This section describes the annotation we could use to define in properties of an object.
 * - @c @QE.ORM.AUTO_INCREMENT : If it is @c true, the property is marked as an @c auto_increment. In this case, 
 * 	QE ORM considers 0 value as a @c null, so database will generate a proper value. After the insertion, 
 * 	this object property will be updated with the new value of the insertion. By default, it is @c false.
 * - @c @QE.ORM.DB_COLUMN : It is the name of the column that will be used to store this property. By default,
 * 	QE ORM will use the name of the property as a column name.
 * - @c @QE.ORM.MAX_LENGTH : In @c QString properties, you can specify the maximun length for database column. If you do not 
 * 	assign any one, the underline SQL database will be used to defined the real type for the database column. For instance,
 * 	if SQLite is used as database, and you do not define any value for this, a @c TEXT column type will be used to store this property.
 * - @c @QE.ORM.NULL : If this is @c false, the column table will be created as @c NOT @c NULL. By default, it is @c true.
 */
#pragma once
#include <QEOrmModel.hpp>
#include <DBDriver/SQLGenerator.hpp>
#include <QECommon/QES11n.hpp>
#include <QSet>
#include <memory>
#include <map>
#include <stack>
#include <mutex>

QE_BEGIN_NAMESPACE

class QEOrm 
{
	public:
		static QEOrm& instance();

		QEOrmModelShd getModel( const QMetaObject *metaObject) const;

		void save( QObject *const source, std::stack<QObject*> context 
				= std::stack<QObject*>()) const;
		void load( const QVariantList pk, QObject* target,
				std::stack<QObject*> context = std::stack<QObject*>()) const;

	private:
		QEOrm();
		Q_DISABLE_COPY(QEOrm)
		void checkAndCreateDBTable( const QEOrmModelShd& model) const;
		
		void insertObjectOnDB(QObject *source, 
				const std::stack<QObject*>& context, const QEOrmModel &model) const;
		void updateObjectOnDB(const QObject *source, 
				const std::stack<QObject*>& context, const QEOrmModel& model) const;
		void saveOneToMany(QObject *source, 
				std::stack<QObject*>& context, const QEOrmModelShd& model) const;
		void loadOneToMany( QObject* target, std::stack<QObject*>& context, 
				const QEOrmModelShd& model) const;


		bool existsObjectOnDB(const QObject *source, const QEOrmModel& model) const;
		QString generateCreateTableIfNotExists( const QEOrmModel& model) const;

	private:
		mutable std::mutex m_cachedModelsMtx;
		mutable std::map<const QMetaObject*, QEOrmModelShd> m_cachedModels;
		mutable std::mutex m_cachedCheckedTablesMtx;
		mutable QSet<QString> m_cachedCheckedTables;
		
		std::unique_ptr<SQLGenerator> m_sqlGenerator;

		// Singleton
		static std::unique_ptr<QEOrm> m_instance;
		static std::once_flag m_onceFlag;
};

QE_END_NAMESPACE
