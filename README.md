# QEOrm
ORM library based on Annotation over Qt Object.
The main target of this project if to provide a FREE and open-source ORM over Qt. 
It uses the powerful MOC processor as a source of annotation.

# How use it
The best way to show how it is work is to start with a simple example. Let's assume we have a 'User' class:

```c++
class Person : public QObject
{
  Q_OBJECT
  Q_PROPERTY( QString name MEMBER name)
  Q_PROPERTY( QString address MEMBER address)
  
  public:
    Person( QObject* parent = nullptr) : QObject( parent){}
    
    QString name;
    QString address;
};
```

## Requirements
 - Support a constructor with first argument as a QObject * parent 

# How to integrate into your project
The best way to integrate this into your project is using Conan.io package
manager and its CMake generator. Let's do it step by step.

	1. Add the 



# Features

# TODO
