<?php
/*+***********************************************************************************
 * The contents of this file are subject to the vtiger CRM Public License Version 1.0
 * ("License"); You may not use this file except in compliance with the License
 * The Original Code is:  vtiger CRM Open Source
 * The Initial Developer of the Original Code is vtiger.
 * Portions created by vtiger are Copyright (C) vtiger.
 * All Rights Reserved.
 *************************************************************************************/

/**
 * Create and trigger events
 */
class VTEventsManager {

	public function __construct($adb) {
		$this->adb = $adb;
	}

	/**
	 * Register an event handler
	 *
	 * @param string The name of the event to handle
	 * @param string The path to the php file containing the handler
	 * @param string The name of the Event Handler class
	 * @param string A condition which must evaluate true for the event to be accepted
	 * @param string list of other events that must be executed before this one
	 */
	public function registerHandler($forEvent, $path, $className, $condition = '', $depedentOn = '[]') {
		$adb = $this->adb;
		$result = $adb->pquery(
			'SELECT is_active FROM vtiger_eventhandlers WHERE event_name=? AND handler_path=? AND handler_class=? limit 1',
			array($forEvent, $path, $className)
		);
		if ($adb->num_rows($result)===0) {
			$handlerId = $adb->getUniqueId('vtiger_eventhandlers');
			$adb->pquery(
				'insert into vtiger_eventhandlers (eventhandler_id, event_name, handler_path, handler_class, cond, is_active, dependent_on) values (?,?,?,?,?, true, ?)',
				array($handlerId, $forEvent, $path, $className, $condition, $depedentOn)
			);
			$this->clearTriggerCache($forEvent);
		}
	}

	/**
	 * Initialize Event Trigger Cache for the required event types.
	 *
	 * @param Object Optional String or Array of event_names for initializing
	 * @param boolean Optional Force the initialization of cache
	 */
	public function initTriggerCache($for = false, $force = false) {
		VTEventTrigger::initCache($for, $force);
	}

	/**
	 * Clear the Event Trigger Cache
	 *
	 * @param Object $forEvent
	 */
	public function clearTriggerCache($forEvent = false) {
		VTEventTrigger::clearCache($forEvent);
	}

	/**
	 * Set an event handler as inactive
	 * @param string handler class to set as inactive
	 *
	 */
	public function setHandlerInActive($handlerClass) {
		$adb = $this->adb;
		$adb->pquery('update vtiger_eventhandlers set is_active=false where handler_class=?', array($handlerClass));
		$this->clearTriggerCache();
	}

	/**
	 * Set an event handler as active
	 *
	 * @param string handler class to set as active
	 */
	public function setHandlerActive($handlerClass) {
		$adb = $this->adb;
		$adb->pquery('update vtiger_eventhandlers set is_active=true where handler_class=?', array($handlerClass));
		$this->clearTriggerCache();
	}

	/**
	 * Unregister a registered handler
	 *
	 * @param string The name of teh Event Handler class to unregister
	 */
	public function unregisterHandler($className) {
		$adb = $this->adb;
		$adb->pquery('delete from vtiger_eventhandlers where handler_class=?', array($className));
		$adb->pquery('delete from vtiger_eventhandler_module where handler_class=?', array($className));
		$this->clearTriggerCache();
	}

	/**
	 * Get an event triger instance
	 *
	 * @param string name of the event
	 * @return VTEventTrigger trigger object for the event
	 */
	public function getTrigger($triggerName) {
		$adb=$this->adb;
		return new VTEventTrigger($adb, $triggerName);
	}

	/**
	 * Trigger an event
	 *
	 * @param string name of the event
	 * @return VTEventTrigger trigger object for the event
	 */
	public function triggerEvent($triggerName, $data) {
		$this->getTrigger($triggerName)->trigger($data);
	}

	/**
	 * Set the module the handler belongs to
	 *
	 * @param string name of the module
	 * @param string name of the handler class
	 */
	public function setModuleForHandler($moduleName, $handlerClass) {
		$adb = $this->adb;
		$result = $adb->pquery('SELECT eventhandler_module_id FROM vtiger_eventhandler_module WHERE handler_class=? limit 1', array($handlerClass));
		if ($adb->num_rows($result)===0) {
			$handlerModuleId = $adb->getUniqueId('vtiger_eventhandler_module');
			$adb->pquery(
				'insert into vtiger_eventhandler_module (eventhandler_module_id, module_name, handler_class) values (?,?,?)',
				array($handlerModuleId, $moduleName, $handlerClass)
			);
		}
	}

	/**
	 * List handler classes for a module
	 *
	 * @param string name of the module
	 */
	public function listHandlersForModule($moduleName) {
		$adb = $this->adb;
		$result = $adb->pquery('SELECT handler_class FROM vtiger_eventhandler_module WHERE module_name=?', array($moduleName));
		$it = new SqlResultIterator($adb, $result);
		$arr = array();
		foreach ($it as $row) {
			$arr[] = $row->handler_class;
		}
		return $arr;
	}

	/**
	 * List active events.
	 *
	 * @return array list of registered events
	 */
	public function listActiveEventHandlers() {
		$adb = $this->adb;
		$result = $adb->pquery('select * from vtiger_eventhandlers where is_active=true', array());
		return $this->listEventHandlers($result);
	}

	public function listAllEventHandlers() {
		$adb = $this->adb;
		$result = $adb->pquery('select * from vtiger_eventhandlers', array());
		return $this->listEventHandlers($result);
	}

	private function listEventHandlers($result) {
		$adb = $this->adb;
		$it = new SQLResultIterator($adb, $result);
		$out = array();
		foreach ($it as $row) {
			$el = array();
			$el['eventName'] = $row->event_name;
			$el['handlerPath'] = $row->handler_path;
			$el['handlerClass'] = $row->handler_class;
			$el['condition'] = $row->cond;
			$el['isActive'] = $row->is_active;
			$out[] = $el;
		}
		return $out;
	}
}
?>
