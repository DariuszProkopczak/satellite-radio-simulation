#include <iostream>

//
// Visitor mthods used with signal processors can be implemented here
//

struct PrintTxProcParams : TxProcessingVisitor {
    virtual void visit(TxProcessing & proc) const override {
        cout << "Abstract Tx Processor. Frequency: " << proc.get_frequency() 
             << " Time: " << proc.get_time() << endl;
    }
    virtual void visit(TxAMProcessing & proc) const override {
        cout << " Tx AM Processor. Frequency: " << proc.get_frequency() 
             << " Time: " << proc.get_time()
             << " Amplitude Sensitivity: " << proc.get_m() 
             << " A: " << proc.get_A() << endl;
    }
    virtual void visit(TxFMProcessing & proc) const override {
        cout << " Tx AM Processor. Frequency: " << proc.get_frequency() 
             << " Time: " << proc.get_time()
             << " Frequency Deviation: " << proc.get_dev() << endl;
    }
};

struct PrintRxProcParams : RxProcessingVisitor {
    virtual void visit(RxProcessing & proc) const override {
        cout << "Abstract Rx Processor. Frequency: " << proc.get_frequency() 
             << " Time: " << proc.get_time() << endl;
    }
    virtual void visit(RxAMProcessing & proc) const override {
        cout << " Rx AM Processor. Frequency: " << proc.get_frequency() 
             << " Time: " << proc.get_time() << endl;
    }
    virtual void visit(RxFMProcessing & proc) const override {
        cout << " Tx AM Processor. Frequency: " << proc.get_frequency() 
             << " Time: " << proc.get_time()
             << " Frequency Deviation: " << proc.get_dev() << endl;
    }
};
