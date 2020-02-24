pub use values::AllowedValues as JSONValues;

use crate::struct_defs::{ArrayContext, ArrayHolder, ObjectContext, ObjectHolder};

pub fn array() -> struct_defs::ArrayContext<()> {
    ArrayContext {
        parent: (),
        holder: ArrayHolder { data: vec![] },
    }
}

pub fn object() -> struct_defs::ObjectContext<()> {
    ObjectContext {
        parent: (),
        holder: ObjectHolder { data: vec![] },
    }
}

mod misc_tools {
    pub trait ToStringIfNecessary {
        fn to_string_if_necessary(self) -> String;
    }

    impl ToStringIfNecessary for String {
        fn to_string_if_necessary(self) -> String {
            self
        }
    }

    impl ToStringIfNecessary for &str {
        fn to_string_if_necessary(self) -> String {
            self.to_string()
        }
    }
}

mod values {
    use crate::struct_defs::*;

    pub enum AllowedValues {
        Integer(i64),
        String(String),
        Boolean(bool),
        Object(ObjectHolder),
        Array(ArrayHolder),
        None,
    }

    impl From<i64> for AllowedValues {
        fn from(value: i64) -> Self {
            AllowedValues::Integer(value)
        }
    }

    impl From<bool> for AllowedValues {
        fn from(value: bool) -> Self {
            AllowedValues::Boolean(value)
        }
    }

    impl From<String> for AllowedValues {
        fn from(value: String) -> Self {
            AllowedValues::String(value)
        }
    }

    impl From<ArrayHolder> for AllowedValues {
        fn from(value: ArrayHolder) -> Self {
            AllowedValues::Array(value)
        }
    }

    impl From<ObjectHolder> for AllowedValues {
        fn from(value: ObjectHolder) -> Self {
            AllowedValues::Object(value)
        }
    }

    impl From<()> for AllowedValues {
        fn from(_: ()) -> Self {
            AllowedValues::None
        }
    }
}

mod struct_defs {
    use crate::values::*;

    pub type ArrayHolderContainer = Vec<AllowedValues>;

    pub struct ArrayHolder {
        pub(crate) data: ArrayHolderContainer
    }

    pub type ObjectHolderContainer = Vec<(String, AllowedValues)>;

    pub struct ObjectHolder {
        pub(crate) data: ObjectHolderContainer
    }

    pub struct ArrayContext<ParentType> {
        pub(crate) parent: ParentType,
        pub(crate) holder: ArrayHolder,
    }

    pub struct ObjectContext<ParentType> {
        pub(crate) parent: ParentType,
        pub(crate) holder: ObjectHolder,
    }

    pub struct ObjectContextProxy<ParentType> {
        pub(crate) data: Option<(String, ObjectContext<ParentType>)>
    }
}

mod trait_defs {
    use crate::struct_defs::*;
    use crate::values::*;

    pub trait GetContainer {
        type Container;
        fn get_container(self) -> Self::Container;
    }

    pub trait GetHolder: GetContainer {
        type Holder;
        fn get_holder(self) -> Self::Holder;
    }

    pub trait GetParent<Parent> {
        type Parent;
        fn get_parent(self) -> Parent;
    }

    pub trait Context<Parent>: GetHolder + GetParent<Parent> {
        fn unpack(self) -> (Parent, Self::Holder);
    }

    pub trait ValueFillingProxy<ReturnType, ValueType> where AllowedValues: From<ValueType> {
        type ReturnType;
        fn fill_value(self, value: ValueType) -> ReturnType;
    }

    pub trait IsArrayContext {}

    pub trait IsObjectContext {}

    pub trait FillKey<KeyType>: IsObjectContext {
        type ReturnProxy;
        fn fill_key(self, key: KeyType) -> Self::ReturnProxy;
    }

    pub trait BeginArray<ReturnType>: ValueFillingProxy<ReturnType, ArrayHolder> where Self: Sized {
        fn begin_array(self) -> ArrayContext<Self>;
    }

    pub trait EndArray<Proxy, ReturnType>: IsArrayContext where Proxy: BeginArray<ReturnType> {
        fn end_array(self) -> ReturnType;
    }

    pub trait BeginObject<ReturnType>: ValueFillingProxy<ReturnType, ObjectHolder>
        where Self: Sized {
        fn begin_object(self) -> ObjectContext<Self>;
    }

    pub trait EndObject<Proxy, ReturnType>: IsObjectContext where Proxy: BeginObject<ReturnType> {
        fn end_object(self) -> ReturnType;
    }
}

pub mod json_manager {
    use crate::misc_tools::*;
    use crate::struct_defs::*;
    use crate::trait_defs::*;
    use crate::values::*;

    impl GetContainer for ArrayHolder {
        type Container = ArrayHolderContainer;

        fn get_container(self) -> Self::Container {
            self.data
        }
    }

    impl GetContainer for ObjectHolder {
        type Container = ObjectHolderContainer;

        fn get_container(self) -> Self::Container {
            self.data
        }
    }

    impl<Parent> GetContainer for ArrayContext<Parent> {
        type Container = ArrayHolderContainer;

        fn get_container(self) -> Self::Container {
            self.holder.get_container()
        }
    }

    impl<Parent> GetContainer for ObjectContext<Parent> {
        type Container = ObjectHolderContainer;

        fn get_container(self) -> Self::Container {
            self.holder.get_container()
        }
    }

    impl<Parent> GetHolder for ArrayContext<Parent> {
        type Holder = ArrayHolder;
        fn get_holder(self) -> Self::Holder {
            self.holder
        }
    }

    impl<Parent> GetHolder for ObjectContext<Parent> {
        type Holder = ObjectHolder;
        fn get_holder(self) -> Self::Holder {
            self.holder
        }
    }

    impl<Parent> GetParent<Parent> for ArrayContext<Parent> {
        type Parent = Parent;
        fn get_parent(self) -> Parent {
            self.parent
        }
    }

    impl<Parent> GetParent<Parent> for ObjectContext<Parent> {
        type Parent = Parent;
        fn get_parent(self) -> Parent {
            self.parent
        }
    }

    impl<Parent> Context<Parent> for ArrayContext<Parent> {
        fn unpack(mut self) -> (Parent, Self::Holder) {
            (self.parent, self.holder)
        }
    }

    impl<Parent> Context<Parent> for ObjectContext<Parent> {
        fn unpack(mut self) -> (Parent, Self::Holder) {
            (self.parent, self.holder)
        }
    }

    impl<Parent> IsArrayContext for ArrayContext<Parent> {}

    impl<Parent> IsObjectContext for ObjectContext<Parent> {}

    impl<Key, Parent> FillKey<Key> for ObjectContext<Parent> where Key: ToStringIfNecessary {
        type ReturnProxy = ObjectContextProxy<Parent>;

        fn fill_key(mut self, key: Key) -> Self::ReturnProxy {
            Self::ReturnProxy {
                data: Some((key.to_string_if_necessary(), self))
            }
        }
    }

    impl<Parent, Value> ValueFillingProxy<ObjectContext<Parent>, Value>
    for ObjectContextProxy<Parent>
        where AllowedValues: From<Value> {
        type ReturnType = ObjectContext<Parent>;

        fn fill_value(mut self, value: Value) -> ObjectContext<Parent> {
            let (mut key, mut object) = self.data.take().unwrap();
            object.holder.data.push((key, AllowedValues::from(value)));
            object
        }
    }

    impl<Parent> Drop for ObjectContextProxy<Parent> {
        fn drop(&mut self) {
            match self.data.take() {
                Some(
                    (mut key, mut object)
                ) => object.holder.data.push((key, AllowedValues::None)),
                _ => ()
            }
        }
    }

    impl<Parent, Value> ValueFillingProxy<ArrayContext<Parent>, Value> for ArrayContext<Parent>
        where AllowedValues: From<Value> {
        type ReturnType = Self;

        fn fill_value(mut self, value: Value) -> ArrayContext<Parent> {
            self.holder.data.push(AllowedValues::from(value));
            self
        }
    }

    impl<Proxy, ReturnType> BeginArray<ReturnType> for Proxy
        where Proxy: ValueFillingProxy<ReturnType, ArrayHolder> {
        fn begin_array(self) -> ArrayContext<Self> {
            ArrayContext {
                parent: self,
                holder: ArrayHolder { data: vec![] },
            }
        }
    }

    impl<Proxy, ReturnType> EndArray<Proxy, ReturnType> for ArrayContext<Proxy>
        where Proxy: ValueFillingProxy<ReturnType, ArrayHolder> {
        fn end_array(self) -> ReturnType {
            let (mut proxy, mut holder) = self.unpack();
            proxy.fill_value(holder)
        }
    }

    impl<Proxy, ReturnType> BeginObject<ReturnType> for Proxy
        where Proxy: ValueFillingProxy<ReturnType, ObjectHolder> {
        fn begin_object(self) -> ObjectContext<Self> {
            ObjectContext {
                parent: self,
                holder: ObjectHolder { data: vec![] },
            }
        }
    }

    impl<Proxy, ReturnType> EndObject<Proxy, ReturnType> for ObjectContext<Proxy>
        where Proxy: ValueFillingProxy<ReturnType, ObjectHolder> {
        fn end_object(self) -> ReturnType {
            let (mut proxy, mut holder) = self.unpack();
            proxy.fill_value(holder)
        }
    }
}

#[cfg(test)]
mod tests {
    #[test]
    fn it_works() {
        // TODO
    }
}