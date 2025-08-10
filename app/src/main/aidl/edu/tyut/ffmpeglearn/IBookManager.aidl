// IBookManager.aidl
package edu.tyut.ffmpeglearn;

// Declare any non-default types here with import statements

import edu.tyut.ffmpeglearn.bean.Book;

interface IBookManager {
    /**
     * Demonstrates some basic types that you can use as parameters
     * and return values in AIDL.
     */
    List<Book> getBookList();
    void addBook(in Book book);
}