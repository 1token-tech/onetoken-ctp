#pragma once

template <typename T>
class Singleton {
 public:
  static T* GetInstance();
  static void destroy();

 private:
  Singleton(Singleton const&){};
  Singleton& operator=(Singleton const&){};

 protected:
  static T* m_instance;

  Singleton() { m_instance = static_cast<T*>(this); };
  ~Singleton(){};
};

template <typename T>
T* Singleton<T>::m_instance = 0;

template <typename T>
T* Singleton<T>::GetInstance() {
  if (!m_instance) {
    Singleton<T>::m_instance = new T();
  }

  return m_instance;
}

template <typename T>
void Singleton<T>::destroy() {
  delete Singleton<T>::m_instance;
  Singleton<T>::m_instance = 0;
}
