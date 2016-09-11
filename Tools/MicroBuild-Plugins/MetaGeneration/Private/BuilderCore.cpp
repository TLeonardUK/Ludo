/*
Ludo Game Engine
Copyright (C) 2016 TwinDrills

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "PCH.h"

#include "BuildTool/Private/BuilderCore.h"
#include "BuildTool/Private/Reflection/BuildReflectionCommand.h"
#include "BuildTool/Private/Reflection/LinkReflectionCommand.h"
#include "BuildTool/Private/Command.h"

namespace Ludo {

LD_EXPORT_ENGINE_CORE(BuilderCore);

Error BuilderCore::Init(GameEngine* Engine)
{
	m_Engine = Engine;

	return Error(ErrorType::Success);
}

Error BuilderCore::Term()
{
	return Error(ErrorType::Success);
}

void BuilderCore::PrintHelp(const Array<Command*>& Commands)
{
	Log(LogTool, LogInfo, "");
	Log(LogTool, LogInfo, "Expected command line formats:");

	for (Command* Cmd : Commands)
	{
		Cmd->PrintHelp();
	}

	Log(LogTool, LogInfo, "");
}


META(DisplayName("My Class"))
class MyClass
{
	GENERATED_BODY();

public:

};


/*
//Meta(DisplayName("My Class"), Range(0, 10))
class MyClass
{
public:
	int m_x;
	int m_y;

	MyClass()
		: m_x(0)
		, m_y(0)
	{
	}

	MyClass(int x, int y)
		: m_x(x)
		, m_y(y)
	{
	}
};

class DisplayNameAttribute : public Attribute
{
};

class MyClassConstructor1Meta : public Method
{
public:
	MyClassConstructor1Meta()
	{
		m_Name = StringId::Create("MyClass");
		m_MetaClassName = StringId::Create("MyClassConstructor1Meta");
		m_FullName = StringId::Create("Ludo::MyClass::MyClass");
		m_UUID = m_FullName;
	}

	virtual void OnPostRegistration() override
	{
		//m_Parameters.Add(LD_ARENA_NEW(ReflectionMemoryArena, Parameter, TypeOfStatic(int), StringId::Create("x")));
		//m_Parameters.Add(LD_ARENA_NEW(ReflectionMemoryArena, Parameter, TypeOfStatic(int), StringId::Create("y")));
		m_ReturnType = TypeOfStatic(MyClass*);
	}

	virtual void InvokeInternal(void* Instance, void* Result, Array<void*>& Parameters) override
	{
		*reinterpret_cast<MyClass**>(Result) = new(Instance) MyClass(
			*reinterpret_cast<int*>(Parameters[0]),
			*reinterpret_cast<int*>(Parameters[1])
		);
	}
};

class MyClassXMeta : public Field
{
public:
	MyClassXMeta()
	{
		m_Name = StringId::Create("m_x");
		m_MetaClassName = StringId::Create("MyClassXMeta");
		m_FullName = StringId::Create("Ludo::MyClass::m_x");
		m_UUID = m_FullName;
	}

	virtual void OnPostRegistration() override
	{
		m_DataType = TypeOfStatic(int);
	}

	virtual void SetInternal(void* Instance, void* Data) override
	{
		reinterpret_cast<MyClass*>(Instance)->m_x = *reinterpret_cast<int*>(Data);
	}

	virtual void GetInternal(void* Instance, void* Data) override
	{
		*reinterpret_cast<int*>(Data) = reinterpret_cast<MyClass*>(Instance)->m_x;
	}
};

class MyClassMeta : public Class
{
public:
	MyClassMeta()
	{
		m_Name = StringId::Create("MyClass");
		m_MetaClassName = StringId::Create("MyClassMeta");
		m_FullName = StringId::Create("Ludo::MyClass");
		m_UUID = m_FullName;
		m_TypeInfoName = StringId::Create(typeid(MyClass).name());
	}

	virtual void OnPostRegistration() override
	{
		m_Methods.Add(static_cast<Method*>(TypeOfStatic(MyClassConstructor1Meta)));
		m_Fields.Add(static_cast<Field*>(TypeOfStatic(MyClassXMeta)));
		m_Attributes.Add(LD_ARENA_NEW(ReflectionMemoryArena, DisplayNameAttribute));
	}

	virtual void* NewInstance(IAllocator* Allocator) override
	{
		return Allocator->Alloc(sizeof(MyClass));
	}
};
*/

int BuilderCore::Run()
{
	/*
	Type::RegisterType<MyClassMeta>();
	Type::RegisterType<MyClassConstructor1Meta>();
	Type::RegisterType<MyClassXMeta>();
	Type::FinalizeRegistration();  

	Class* classType = (Class*)TypeOfStatic(MyClass);

	MyClass* instance2 = classType->New<ReflectionMemoryArena, MyClass>(1, 2);
	//MyClass* instance1 = classType->New<ReflectionMemoryArena, MyClass>(1);
	//MyClass* instance0 = classType->New<ReflectionMemoryArena, MyClass>();

	Method* method = classType->FindMethod(StringId::Create("MyClass"), TypeOfStatic(MyClass*), TypeOfStatic(int), TypeOfStatic(int));
	DisplayNameAttribute* Attr = classType->FindAttribute<DisplayNameAttribute>();


	Field* field = classType->FindField(StringId::Create("m_x"), TypeOfStatic(int));
	field->SetValue<int>(instance2, 3);
	int x = field->GetValue<int>(instance2);

	instance2 = method->Invoke<MyClass*>(instance2, 5, 6);
	
	//classType->Clone(instance2);
	//classType->DeepClone(instance2);

//	method->Invoke(instance, 1, 2);

//	Attribute* attr = classType->FindAttribute<DisplayNameAttribute>()->Name;

	Type::Teardown();


	BuildReflection Build Core "D:\Git\Ludo\Engine\Source\Engine\Core" "D:\Git\Ludo\Build\ProjectFiles\Core\Generated" -ID:\Git\Ludo\Engine\Source -ID:\Git\Ludo\Engine\Source\Engine -ID:\Git\Ludo\Engine\Source\ThirdParty -ID:\Git\Ludo\Engine\Source\Binaries -ID:\Git\Ludo\Engine\Source\Tools -DLD_SHIPPING_BUILD -DLD_PLATFORM_WINDOWS -DLD_ARCHITECTURE_X64 -DWIN32 -D_HAS_ITERATOR_DEBUGGING=0 -D_SECURE_SCL=0 -DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS -D_NO_DEBUG_HEAP=1
*/

	Array<Command*> Commands;
 	Commands.Add(LD_ARENA_NEW(LibCMemoryArena, BuildReflectionCommand, m_Engine));
	Commands.Add(LD_ARENA_NEW(LibCMemoryArena, LinkReflectionCommand, m_Engine));

	Array<String> Args = Environment::GetCommandLineArguments();
	if (Args.Length() == 0)
	{
		Log(LogTool, LogWarning, "No command line argument specified, cannot continue.");
		PrintHelp(Commands);

		return 1;
	}
	else
	{
		String CommandName = Args[0];
		Args.RemoveAt(0);

		for (Command* Cmd : Commands)
		{
			if (Cmd->GetName() == CommandName)
			{
				if (Cmd->ValidateArgs(Args))
				{
					if (!Cmd->Run(Args))
					{
						LogF(LogTool, LogError, "Command '%s' failed to run.", CommandName.Data());
						return 1;
					}
					else
					{
						return 0;
					}
				}
				else
				{
					LogF(LogTool, LogError, "Incorrect or invalid arguments for command '%s'.", CommandName.Data());
					PrintHelp(Commands);
					return 1;
				}
			}
		}

		LogF(LogTool, LogError, "Unknown command '%s'.", CommandName.Data());
		PrintHelp(Commands);
		return 1;
	}

	return 0;
}

bool BuilderCore::UseFastInit() 
{
	return true;
}

}; // namespace Ludo