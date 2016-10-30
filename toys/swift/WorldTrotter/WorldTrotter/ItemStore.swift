//
//  ItemStore.swift
//  WorldTrotter
//
//  Created by Bart Bartel on 10/30/16.
//  Copyright © 2016 Bart Bartel. All rights reserved.
//

import UIKit

class ItemStore {
    var allItems = [Item]()
    
    //init() {
    //    for _ in 0..<5 {
    //        createItem()
    //    }
    //}
    func moveItemAtIndex(fromIndex: Int, toIndex: Int) {
        if fromIndex == toIndex {
            return
        }
        
        // get reference to object being moved so you can reinsert it
        let movedItem = allItems[fromIndex]
        
        // remove item from array
        allItems.remove(at: fromIndex)
        
        // insert item in array at new location
        allItems.insert(movedItem, at: toIndex)
    }
    
    func removeItem(item: Item) {
        if let index = allItems.index(of: item) {
            allItems.remove(at: index)
        }
    }
    
    func createItem() -> Item {
        let newItem = Item(random: true)
        
        allItems.append(newItem)
        
        return newItem
    }
    
}
