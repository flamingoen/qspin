#include "verificationoutput.h"


VerificationOutput::VerificationOutput()
{
    spinVer = "";
    eval = "RED";
    never = "-";
    acceptance = "-";
    assertion = "-";
    invalid = "-";
    partial = "-";


}

void VerificationOutput::processVerification(QString output){

    std::cout<<output.toStdString() + " stds"  << std::endl;

    //TODO: Husk exception handling til at (især vores regex)
    //TODO: Laves der verificationOutput klasse, så husk at give timestamp så man kan se hvornår den sidste er kørt.

    // SPIN VERSION
    QRegularExpression re("(Spin Version .+)\\)");
    QRegularExpressionMatch spinVerM = re.match(output);
    spinVer = spinVerM.captured(1);

    // EVALUATION
    re.setPattern("errors: (\\d)");
    QRegularExpressionMatch evalM = re.match(output);
    if(evalM.captured(1) == "0"){
        eval = "GREEN";
    }else{
        eval = "RED";
    }

    // STATESPACEPROP
    re.setPattern("([-+])\\s*Partial Order Reduction");
    QRegularExpressionMatch partialM = re.match(output);
    partial = partialM.captured(1);


    re.setPattern("never\\s*claim\\s* (.+)");
    QRegularExpressionMatch neverM = re.match(output);
    never = neverM.captured(1);

    re.setPattern("assertion\\s*violations\\s*(.+)");
    QRegularExpressionMatch assertionM = re.match(output);
    assertion = assertionM.captured(1);

    re.setPattern("(\\S*)\\s*cycles\\s*(.+)");
    QRegularExpressionMatch acceptanceM = re.match(output);
    acceptanceType = acceptanceM.captured(1).replace(0,1,acceptanceM.captured(1)[0].toUpper()); // TO ENSURE THAT THE FIRST CHARACTER IS UPPERCASE.
    acceptance = acceptanceM.captured(2);

    re.setPattern("invalid\\s+end\\s*states\\s*(.+)");
    QRegularExpressionMatch invalidM = re.match(output);
    invalid = invalidM.captured(1);

    // STATESPACESPECS
    // ERRORS - already used in the evaluation above.
    errors = evalM.captured(1);

    // DEPTH
    re.setPattern("depth reached (\\d*)");
    QRegularExpressionMatch depthM = re.match(output);
    depth = depthM.captured(1);


    // STORED STATES
    re.setPattern("(\\d*) states, stored");
    QRegularExpressionMatch storedStatesM = re.match(output);
    storedStates = storedStatesM.captured(1);

    // MATCHED STATES
    re.setPattern("(\\d*) states, matched");
    QRegularExpressionMatch matchedStatesM = re.match(output);
    matchedStates = matchedStatesM.captured(1);

    // TRANSITIONS
    re.setPattern("(\\d*) transitions");
    QRegularExpressionMatch transitionsM = re.match(output);
    transitions = transitionsM.captured(1);

    // ATOMIC STATES
    re.setPattern("(\\d*) atomic steps");
    QRegularExpressionMatch atomicM = re.match(output);
    atomic = atomicM.captured(1);

    // STATE SIZE
    re.setPattern("State-vector (\\d*) byte");
    QRegularExpressionMatch statesizeM = re.match(output);
    statesize = statesizeM.captured(1);

    // HASH CONFLICTS
    re.setPattern("hash conflicts:\\s* (\\d*)");
    QRegularExpressionMatch hashconflictM = re.match(output);
    hashconflict = hashconflictM.captured(1);

    // HASH SIZE
    re.setPattern("max size (2\\^\\d*) states");
    QRegularExpressionMatch hashsizeM = re.match(output);
    hashsize = hashsizeM.captured(1);






}
