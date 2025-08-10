package edu.tyut.ffmpeglearn.bean

import android.net.Uri
import android.os.Parcel
import android.os.Parcelable
import androidx.core.os.ParcelCompat

@ConsistentCopyVisibility
internal data class Person internal constructor(
    private val name: String,
    private val age: Int,
    private val isMan: Boolean
) : Parcelable {

    private constructor(parcel: Parcel?) : this(
        name = parcel?.readString() ?: "",
        age = parcel?.readInt() ?: 0,
        isMan = parcel?.readByte() == 1.toByte()
    )

    companion object CREATOR : Parcelable.Creator<Person>{
        override fun createFromParcel(source: Parcel?): Person? {
            return Person(source)
        }

        override fun newArray(size: Int): Array<out Person?>? {
            return arrayOfNulls(size)
        }
    }
    override fun describeContents(): Int {
        return 0
    }

    override fun writeToParcel(dest: Parcel, flags: Int) {
        dest.writeString(this.name)
        dest.writeInt(this.age)
        dest.writeByte(if(this.isMan) 1.toByte() else 0.toByte())
        ParcelCompat.readParcelable<Uri>(dest, Thread.currentThread().contextClassLoader, Uri::class.java)
        dest.writeParcelable(Uri.EMPTY, 0)
    }
}
