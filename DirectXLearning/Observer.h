#pragma once

#include <string>
#include <list>
#include <memory>

namespace Obs {
	class IObserver {
	public:
		virtual ~IObserver() {};
		virtual void Update(const std::string& message) = 0;
	};

	class ISubject {
	public:
		virtual ~ISubject() {};
		virtual void Attach(IObserver *observer) = 0;
		virtual void Detach(IObserver *observer) = 0;
		virtual void Notify() = 0;
	};

	class Subject : public ISubject {
	public:
		virtual ~Subject() {}

		void Attach(IObserver *observer) override {
			observerList.push_back(observer);
		}

		void Detach(IObserver *observer) override {
			observerList.remove(observer);
		}

	protected:
		void Notify() override {
			for (auto itr = observerList.begin(); itr != observerList.end(); itr++) {
				(*itr)->Update(message);
			}
		}

		void CreateMessage(std::string message = "Empty") {
			this->message = message;
			Notify();
		}

	private:
		std::list<IObserver *> observerList;
		std::string message;
	};

	class Observer : public IObserver {
	public:
		Observer(Subject& subject) : subject(subject) {
			this->subject.Attach(this);
		}

		virtual ~Observer() {
		}

		void Update(const std::string& messageFromSubject) override {
			this->messageFromSubject = messageFromSubject;
		}

		void RemoveMeFromTheList() {
			subject.Detach(this);
		}

	protected:
		std::string messageFromSubject;
		Subject& subject;
	};
}
