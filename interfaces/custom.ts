/**
 * @param Loose 
 * @description adds loose autocompletion/dictionary  
 * 
 * @param T is the dictionary part of the type
 * @param K is a string or number
 */
 export type Loose<T extends K, K extends (string | number)> = T | Omit<K, T>
 /**
  * @param Exclusion 
  * @description excludes a type
  * 
  * @example 
  * type e = { image: string, id: string }
  * type v = Exclusion<e, keyof { image: string }> // the same as { id: string }
  * 
  * @param T is the whole type
  * @param K keyof the part that will be excluded
  */
 export type Exclusion<T, K extends keyof T> = Pick<T, Exclude<keyof T, K>>
 
 /**
  * @param ExcludeMatchingProperties
  * @description excludes matching properties
  * 
  * @example type post = { id: number, title: string }
  * type PostExcluded = ExcludeMatchingProperties<post, number> // { id: number }
  * 
  * @param T is the type
  * @param K is the matching property
  */
 export type ExcludeMatchingProperties<T, V> = Pick<
 T,
 { [K in keyof T]-?: T[K] extends V ? never : K }[keyof T]
 >;