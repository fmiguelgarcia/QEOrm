# QEOrm

It is the ORM library of **QE Framework**. It allows you to map *QObjects* in memory to a relational database.
The main target is to provide a FREE and open-source ORM over Qt. 

[![badge](https://img.shields.io/badge/conan.io-QEOrm%2F1.0.0-green.svg?logo=data:image/png;base64%2CiVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAMAAAAolt3jAAAA1VBMVEUAAABhlctjlstkl8tlmMtlmMxlmcxmmcxnmsxpnMxpnM1qnc1sn85voM91oM11oc1xotB2oc56pNF6pNJ2ptJ8ptJ8ptN9ptN8p9N5qNJ9p9N9p9R8qtOBqdSAqtOAqtR%2BrNSCrNJ/rdWDrNWCsNWCsNaJs9eLs9iRvNuVvdyVv9yXwd2Zwt6axN6dxt%2Bfx%2BChyeGiyuGjyuCjyuGly%2BGlzOKmzOGozuKoz%2BKqz%2BOq0OOv1OWw1OWw1eWx1eWy1uay1%2Baz1%2Baz1%2Bez2Oe02Oe12ee22ujUGwH3AAAAAXRSTlMAQObYZgAAAAFiS0dEAIgFHUgAAAAJcEhZcwAACxMAAAsTAQCanBgAAAAHdElNRQfgBQkREyOxFIh/AAAAiklEQVQI12NgAAMbOwY4sLZ2NtQ1coVKWNvoc/Eq8XDr2wB5Ig62ekza9vaOqpK2TpoMzOxaFtwqZua2Bm4makIM7OzMAjoaCqYuxooSUqJALjs7o4yVpbowvzSUy87KqSwmxQfnsrPISyFzWeWAXCkpMaBVIC4bmCsOdgiUKwh3JojLgAQ4ZCE0AMm2D29tZwe6AAAAAElFTkSuQmCC)](http://www.conan.io/source/QEOrm/1.0.0/fmiguelgarcia/stable)

## How to use it

Let's go through an example to learn how to use **QEOrm** library.

### First step: annotate your classes

**QEOrm** uses QObject's properties and their annotations (see [QEAnnotation library](https://github.com/fmiguelgarcia/QEAnnotation)).
Let's assume we have a 'Book' class and we want to store it into DB:

```C++
class Book : public QObject
{
	Q_OBJECT 
	Q_CLASSINFO( "class", "@qe.model.name=book")

	Q_PROPERTY( int id MEMBER id)
	Q_CLASSINFO( "id", "@qe.entity.isAutoIncrementable=true @qe.entity.name=bid")

	Q_PROPERTY( QString title MEMBER title)
	Q_CLASSINFO( "title", "@qe.entity.maxLength=256")

	Q_PROPERTY( QString author MEMBER author)
	Q_CLASSINFO( "author", "@qe.entity.maxLength=512")

	public:
		int id = 0;
		QString title;
		QString author;
};
```

Using those annotations, **QEOrm** will create (if it does not exist yet) a DB schema:
 - Table name will be *book*, as we indicated using *\@qe.model.name*.
 - This table will contain 3 fields:
    - *bid* field is an integer, primary key and auto_increment. When you mark the property as *isAutoIncrementable=true* and there is no primary key, it will become the primary key. In this example, we can also see a name mapping between property name (*id*) to column name (*bid*) using *\@qe.entity.name* annotation.
    - *title* field is a text column which maximum length is 256. The real type depends of the used DB. On SQLite, it will be 'TEXT', but in MySQL it could be 'VARCHAR'.
    - *author* field is also a text column which maximum length is 512.

### Store objects into DB

Insertion and update uses the same function: *QEOrm::instance().save(...)*.
You have to setup the default database *before* use this function. 
You are also able to use a custom database connection using *SerializedItem* object to indicate the name of the connection.

But let's see an example:

```C++
	Book b1;
	b1.id = 0;
	b1.title = "Effective Modern C++";
	b1.author = "Scott Meyers";

	QEOrm::instance().save( &b1);

	// b1.id is automatically updated!!!
	if( b1.id == 0)
		qCritical() << "Error: Book is NOT saved into DB";

	b1.title = "Effective Modern C++ 11";
	QEOrm::instance().save( &b1);
	
	// b1 is updated !!!	
```

### Load objects from DB

In order to load an object, you will need to know the values of the primary key:

```C++
	Book b2;

	QEOrm::instance().load( {b1.id}, &b2);
	
	if( b1 != b2)
		qCritial() << "Error!!!";
```

## More details

### QObject MUST have a Q_INVOKABLE constructor
In order to create and load objects in memory, the target class **MUST** have at least one invokable constructor (using **Q_INVOKABLE** Qt macro) and a *QObject pointer* as parent object as usual in Qt.

```C++
	class PersonInfo : public QObject
	{
		Q_OBJECT

		// Id property is the primary key on DB and auto_increment.
		Q_PROPERTY( int id MEMBER m_id)
		Q_CLASSINFO( "id", "@qe.entity.isAutoIncrementable=true")

		// Name property is NOT null and max size is 256.
		Q_PROPERTY( QString name MEMBER m_name)
		Q_CLASSINFO( "name", "@qe.entity.maxLength=256 @qe.entity.isNullable=false)
		
		public:
			// Invokable constructor with, at least, QObject* as first argument.
			Q_INVOKABLE PersonInfo( QObject* parent = nullptr, QString name = QString());
		
			// ...
	};
```
### Available Annotations

The below table shows the available annotations on **QEOrm library**:

Annotation              | Scope    | Description 
------------------------|----------|------------------------------------------------------
 \@qe.model.name        | class    | DB table name. <br>By default, class name will be used.
 \@qe.entity.primaryKey | class    | A comma-separated list of property names which compose the primary key. <br>By default, it is empty.
 \@qe.entity.name       | property | DB column name. <br>By default, property name will be used.
 \@qe.entity.isEnable   | property | If it is false, that property will NOT be exported into DB. <br>By default, it is true. 
 \@qe.entity.maxLength  | property | In array types (like varchar, char, etc.) it sets the maximum size constraint. <br>By default, it is 0 and means no constraint.
 \@qe.entity.isParentExported    | class    | If it is true, parent class properties will be also exported. <br>By default, it is false.
 \@qe.entity.isNullable          | property | It indicates if DB column could be null. <br>By default, it is true
 \@qe.entity.isAutoIncrementable | property | It marks its DB column as auto_increment field. <br>Due to some DB constraints, this field should be only one, integer type and it will become a primary key if no other entity has been marked as primary key.
 \@qe.entity.mapping.type        | property | It refers the type of the relation. <br>See [Relations One to Many](#relations-one-to-many)
 \@qe.entity.mapping.entity      | property | It sets the target entity in the relation. <br>See [Relations One to Many](#relations-one-to-many)

### Relations One to Many

*TODO*, but you can see an example in source code of tests.
There is a lot of pending-work here... I know, but step by step.

### Enum support
Enum types are supported from Qt 5.5.0 version and it requires the use of **Q_ENUM** in the definition of the type.
You can see an example in source code of tests.

## Installation

The easiest way to use this library in your code is by [Conan Package Manager](https://www.conan.io).
Please use the project setup that you can find in [QEOrm/1.0.0 Conan](https://www.conan.io/source/QEOrm/1.0.0/fmiguelgarcia/stable).

If you use *CMake*, the steps after adding this dependency to your "conanfile.py" are as usual:
```bash
	$~/yourProject > mkdir build
	$~/yourProject > cd build
	$~/yourProject/build > conan install -s build_type=debug .. 
	$~/yourProject/build > cmake -DCMAKE_BUILD_TYPE=Debug  .. 
	$~/yourProject/build > cmake --build .
```

## Versioning

This library uses [Semantic Versioning](htpp://semver.org) and internal private implementation to provide a long-term, stable, and reusable binaries.

## License 

This license follows the dual-license pattern that Qt is currently using. A [LGPL 3.0 license](https://www.gnu.org/licenses/lgpl-3.0-standalone.html) for projects that do not need commercial support, and another one, **Commercial license** for rest of cases. 

You can find a brief comparison between both licenses in the below table:

 License types                 | LGPLv3 | Commercial 
-------------------------------|--------|------------
 Community Support             | Yes    | Yes
 Official Support Helpdesk     | No     | Yes
 Keep your application private | No     | Yes
 Possible to keep your application private with dynamic linking | Yes | Yes
 No need to provide a relinking mechanism for QE Framework <br> (can always use static linking)  | No | Yes
 No need to provide a copy of license and explicitly<br> acknowledge the use of QE | No | Yes
 No need to make copy of the QE Framework source code <br> available for customers | No | Yes
 Full rights to proprietary QE source code modifications | No | Yes
 Enforce DRM                   | [See LGPLv3 FAQ](https://www.gnu.org/licenses/gpl-faq.html#DRMProhibited) | Yes
 Enforce software patents      | [See LGPLv3 FAQ](https://www.gnu.org/licenses/gpl-faq.html#DRMProhibited) | Yes

